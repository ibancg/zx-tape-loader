//-*- C++ -*-

#include <stdlib.h>
#include <linux/soundcard.h>
#include <math.h>
#include <signal.h>

#include "core.h"
#include "filter.h"
#include "tzx.h"
#include "file.h"

bool Core::stop = false;

void Core::ControlC_Callback(int)
{
    stop = true;
}

//----------------------------------------------------------------------------

Core::Core(const char* output_file, const char* input_file)
{
    signal(SIGINT, ControlC_Callback);

    status = s_noise;
    sample_index = 0;

    // band pass Butterworth IIR filter [ 500, 11025 ] MHz.
    FLT filter_a[] = { 1.000000000000000,  -3.813865383597037,
                       5.458723791505597,  -3.474942611565117,
                       0.830107707951727 };
    FLT filter_b[] = { 0.911102468413717,  -3.644409873654869,
                       5.466614810482302,  -3.644409873654869,
                       0.911102468413717 };

    filter = new Filter( 4, 4, filter_a, filter_b );

    file_reader = nullptr;

    file_reader = new FileReader(input_file, &sample_rate);
    tzx_writer = new TzxWriter(output_file);
}

//----------------------------------------------------------------------------

Core::~Core()
{
    delete file_reader;
    delete tzx_writer;
    delete filter;
}

//----------------------------------------------------------------------------

bool Core::run()
{
    bool ok = false;

    printf("Decoding... (press CTRL-C to abort)\n");

    FLT noiseThresholdDb  = 37.0; // dB
    FLT noiseThreshold = pow(10.0, noiseThresholdDb / 10.0);
    FLT pilotTolerance = 0.4;
    int pilotMinEdges = 200;
    FLT bitTolerance = 0.3;

    status = s_noise;
    stop = false;

    edges[0] = edges[1] = edges[2] = 0;

    unsigned long int carrier_counter = 0;
    unsigned long int edge_counter = 0;
    unsigned long int bit_counter[2] = { 0, 0 };

    FLT edge_accum = 0;

    FLT mean, ratio, period, tol;
    FLT pilot_freq, sync1_freq, sync2_freq, bit_freq[2];

    FLT bit_accum[2] = { 0.0, 0.0 };

    unsigned int end_block = 0;

    char flipflop = 0; // bit selector, 0/1

    unsigned char data[131072]; // maximum of 128Kb per block

    while (!stop) {

        FLT sample, edge;
        if (getSample(&sample, &edge) < 0) {
            if (status == s_data) status = s_decode;
            else status = s_noise;

            printf("END of file\n");
            stop = true;
        }

        // carrier detection.
        if (fabs(sample) > noiseThreshold) {

            carrier_counter = CARRIER_COUNTER_PERIOD;

            if (status == s_noise) { // found carrier
                printf("%i: Found carrier\n", sample_index);
                status = s_signal;
                edges[0] = edges[1] = edges[2] = 0;
                edge_counter = 0;
                edge_accum = 0;
                continue;
            }
        } else {
            carrier_counter--;
            if (carrier_counter == 0) { // lost carrier
                printf("%i: Lost Carrier\n", sample_index);
                status = (status == s_data) ? s_decode : s_noise;
            }
        }

        // per edge analysis
        if ((status != s_noise) && (edge > 0)) {

            edges[2] = edges[1];
            edges[1] = edges[0];
            edges[0] = edge;     // shift edge vector.
            edge_counter++;

            /*      printf("edge at sample %i: [%0.2f  %0.2f  %0.2f] (%0.2f  %0.2f)\n",
          sample_index, edges[0], edges[1], edges[2],
          edges[0] - edges[1], edges[1] - edges[2]);*/

            switch (status) {
            case s_signal:

                if (edge_counter >= EDGE_TRANSIT) {
                    edge_accum += edges[0] - edges[1];

                    mean = edge_accum/(edge_counter - EDGE_TRANSIT + 1);
                    ratio = 1.0 - (edges[0] - edges[1])/mean;

                    //	  printf(" L=%0.2f, mean=%0.2f\n", edges[0] - edges[1], mean);

                    if (fabs(ratio) > pilotTolerance) {
                        /*	    printf(" warning, not continuous wave at sample %i, edge %i, tol=%0.2f, searching PILOT\n",
            sample_index, edge_counter, ratio);*/
                        edges[0] = edges[1] = edges[2] = 0;
                        edge_counter = 0;
                        edge_accum = 0; // reset status
                    }

                    if (edge_counter == pilotMinEdges) {

                        pilot_freq = 0.5*sample_rate/mean;
                        /*	    FLT freq_ratio = 1.0 - pilot_freq/PILOT_FREQ;
            if (abs(f_ratio) > TOLERANCE2)  {
            printf(" ERROR, f=%f Hz, r=%f\n", f_pilot, f_ratio);
            edges[0] = edges[1] = edges[2] = 0;
            edge_counter = 0;
            edge_accum = 0; // reset status
            continue;
            }*/

                        if (end_block) {
                            after_block_pause = 1000.0*(sample_index - end_block)/sample_rate;
                            //	      printf(" Pause after block: %0.2f ms\n", after_block_pause);
                            tzx_writer->writeID10(data, global_bit_counter >> 3,
                                                  (unsigned int) after_block_pause);
                        }

                        printf("\n\n %i: found PILOT f=%0.2f Hz\n",
                               sample_index, pilot_freq);
                        end_block = 0;
                        status = s_pilot;
                    }
                }
                continue;

            case s_pilot:

                mean = edge_accum/(edge_counter - EDGE_TRANSIT + 1);
                ratio = 1.0 - (edges[0] - edges[1])/mean;

                if (fabs(ratio) > pilotTolerance) {

                    pilot_freq = 0.5*sample_rate/mean;

                    /*	  printf(" PILOT end at sample %i, l=%0.2f T, f=%0.2f Hz, ratio=%f, %i pulses\n",
         sample_index, 0.5*Z80_FREQ/pilot_freq, pilot_freq,
         ratio, edge_counter);*/

                    sync1_freq = 0.5*sample_rate/(edges[0] - edges[1]);
                    printf(" %i: found SYNC1 f=%0.2f Hz\n", sample_index, sync1_freq);
                    status = s_sync1;
                    continue;
                }

                edge_accum += edges[0] - edges[1];
                continue;

            case s_sync1:

                sync2_freq = 0.5*sample_rate/(edges[0] - edges[1]);
                printf(" %i: found SYNC2 f=%0.2f Hz\n", sample_index, sync2_freq);
                printf(" getting data\n");
                status = s_data;
                edge_counter = 0;
                global_bit_counter = 0;
                bit_accum[0] = bit_accum[1] = 0.0;
                bit_counter[0] = bit_counter[1] = 0;
                flipflop = 0;
                continue;

            case s_data:

                if ((edge_counter & 0x01) == 0) {
                    /*	  sample = (flancos(1) - flancos(3))*F_RATIO;
          periodos = [ periodos sample ];*/
                    period = edges[0] - edges[2]; // whole period

                    if ((global_bit_counter & 0x07) == 0) // multiple of 8
                        data[global_bit_counter >> 3] = 0;

                    if (global_bit_counter > 0) {

                        mean = bit_accum[flipflop]/bit_counter[flipflop];
                        tol = 1 - period/mean;

                        if (fabs(tol) > bitTolerance) {
                            //	      printf(" bit change: tol=%0.2f\n", tol);
                            if (global_bit_counter <= 7) {
                                printf(" %i: ERROR: not uniform MARKER, tol=%0.2f\n",
                                       sample_index, tol);
                                status = s_noise;
                                continue;
                            }

                            static FLT max_tol = 0;
                            if (tol > max_tol) {
                                max_tol = tol;
                                printf("%i: max_tol = %lf\n", sample_index, tol);
                            }

                            flipflop = !flipflop; // bit change
                            mean = bit_accum[flipflop]/bit_counter[flipflop];
                            tol = 1 - period/mean;

                            if (fabs(tol) > 0.8) {
                                printf(" %i: warning: bit error, assuming end, tol=%0.2f\n",
                                       sample_index, tol);
                                status = s_decode;
                            }
                        }
                    }

                    data[global_bit_counter >> 3] <<= 1; // shift the byte
                    data[global_bit_counter >> 3] |= flipflop; // add the bit

                    global_bit_counter++;
                    bit_counter[flipflop]++;
                    bit_accum[flipflop] += period;

                    if ((global_bit_counter & 0x1fff) == 0)
                        printf("  %ik read\n", global_bit_counter >> 13);
                }

                break;
            }
        }

        if (status == s_decode) {

            end_block = sample_index;
            global_bit_counter = (global_bit_counter >> 3) << 3; // trunc to a 8 multiple

            unsigned char checksum = 0;
            bool reverse =
                    (bit_accum[0]/bit_counter[0] > bit_accum[1]/bit_counter[1]);

            for (int i = 0; i < (global_bit_counter >> 3); i++) {
                checksum ^= data[i];
                if (reverse) data[i] ^= 0xff; // reverse bits
                //	printf("%0x ", data[i]);
            }

            flipflop = reverse ? 1 : 0;
            bit_freq[0] = 1.0*sample_rate*bit_counter[flipflop]/bit_accum[flipflop];
            bit_freq[1] = 1.0*sample_rate*bit_counter[!flipflop]/bit_accum[!flipflop];

            printf("\n %i bytes read (%i bits) , f0=%0.2f Hz, f1=%0.2f Hz, checksum=%X ",
                   global_bit_counter >> 3, global_bit_counter, bit_freq[0], bit_freq[1], checksum);
            if (checksum == 0) {
                printf("ok\n");
                ok = true;
            }
            else {
                printf("fail!\n");
                ok = false;
            }
            status = s_noise;
        }
    }

    if (end_block) {
        after_block_pause = 0;
        //	      printf(" Pause after block: %0.2f ms\n", after_block_pause);
        tzx_writer->writeID10(data, global_bit_counter >> 3,
                              (unsigned int) after_block_pause);
    }

    printf("%i samples read\n", sample_index);
    return ok;
}

//----------------------------------------------------------------------------

int Core::getSample(FLT* sample, FLT* edge)
{
    static FLT   old_sample = 0;
    static int   old_sample_index = 0;
    static bool  edge_found;
    SAMPLE_TYPE  new_sample;

    if (file_reader->read(&new_sample, 1*sizeof(SAMPLE_TYPE)) < 1)
        return -1;

    *sample = filter->filter(new_sample);
    //*sample = new_sample;
    sample_index++;

    *edge = -1; // no edge by default

    // to avoid two consecutive edges (when a sample is exactly 0)
    if (edge_found) edge_found = false;
    else if ((*sample)*old_sample <= 0) { // edge
        // lineal interpolation
        *edge = (sample_index - 1 + old_sample/(old_sample - *sample));
        edge_found = true;
    }

    //  printf("%i\t%i\t%lf\n", sample_index, new_sample, *sample);
    old_sample = *sample;
    old_sample_index = sample_index;

    return 0;
}
