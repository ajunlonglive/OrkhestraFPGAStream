#!/usr/bin/perl

use warnings;
use strict;

sub getLoggingTime {

    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);
    my $nice_timestamp = sprintf ( "%04d%02d%02d %02d:%02d:%02d",
                                   $year+1900,$mon+1,$mday,$hour,$min,$sec);
    return $nice_timestamp;
}

sub print_mean_and_dev {
    my @all_times = @_;

    my $mean = 0;
    foreach my $time (@all_times) {
        $mean += $time;
    }
    $mean = $mean / @all_times;
    my $run_count = $#all_times + 1;
    print "Run count: $run_count\n";
    print "Mean: $mean\n";
    my $std_dev = 0;
    foreach my $time (@all_times) {
        $std_dev += ($time - $mean)**2;
    }
    print "Variance: $std_dev\n";
    $std_dev = sqrt($std_dev);
    print "Std Dev: $std_dev\n";
}

#my @input_defs=("benchmark_Q19_SF001.json", "benchmark_Q19_SF002.json", "benchmark_Q19_SF003.json", "benchmark_Q19_SF004.json", "benchmark_Q19_SF005.json", "benchmark_Q19_SF006.json", "benchmark_Q19_SF007.json", "benchmark_Q19_SF008.json","benchmark_Q19_SF009.json","benchmark_Q19_SF01.json", "benchmark_Q19_SF02.json", "benchmark_Q19_SF03.json");
my @input_defs=("benchmark_Q19_SF01.json");
my @input_configs=("fast_benchmark_config.ini");
#my @input_defs=("benchmark_Q19_SF1.json");
#my @input_configs=("big_benchmark_config.ini");

my $warm_up_runs = 3;
my $regular_runs = 10;

system ("unzip -o -qq benchmark_defs.zip");
system ("unzip -o -qq lineitem.zip");
system ("unzip -o -qq part.zip");

foreach my $input_def ( @input_defs ) {
    foreach my $input_config ( @input_configs ) {
        print getLoggingTime() . "\n";
        print "dbmstodspi -c $input_config -i $input_def\n";
        print "$warm_up_runs WARM UP RUNS\n";
        for (my $i = 0; $i < $warm_up_runs; $i++){
            system("sudo", "./dbmstodspi", "-c", "$input_config", "-i", "$input_def", "-q");
        } 
        
        my @accumulated_times = ();
        my @int_and_run_times = ();
        my @acc_data_times = ();
        my @acc_result_times = ();
        my @acc_reconfig_times = ();
        my @acc_reconfig_plus_times = ();
        my @overall_times = ();
        my @module_times = ();
        my $module_count = 0;
        
        print "$regular_runs BENCHMARK RUNS\n";
        for (my $i = 0; $i < $regular_runs; $i++){
            my @current_run_module_times = ();
            my @current_run_data_times = ();
            my @current_run_data_write_times = ();
            my @current_run_reconfiguration_times = ();
            my @current_run_reconfiguration_extra_times = ();
            my @current_init_and_run_times = ();
            my $init_and_run_time = 0;
            my $run_time = 0;
            my $configuration_time = 0;
            my $data_read_time = 0;
            my $data_write_time = 0;
            my $configuration_extra_time = 0;
            my $output = `sudo ./dbmstodspi -c $input_config -i $input_def`;
            foreach my $line (split /[\r\n]+/, $output) {
                if ($line =~ /.*Execution time = (\d+).*/) {
                    push @current_run_module_times, $1;
                }
                if ($line =~ /.*Reconfiguration time = (\d+).*/) {
                    push @current_run_reconfiguration_times, $1;
                }
                if ($line =~ /.*Read data time = (\d+).*/) {
                    push @current_run_data_times, $1;
                }
                if ($line =~ /.*Write result data time = (\d+).*/) {
                    push @current_run_data_write_times, $1;
                }
                if ($line =~ /.*Extra config time = (\d+).*/) {
                    push @current_run_reconfiguration_extra_times, $1;
                }
                if ($line =~ /.*Overall time = (\d+).*/) {
                    push @overall_times, $1;
                }
                if ($line =~ /.*Init and run time = (\d+).*/) {
                    push @current_init_and_run_times, $1;
                }
            }
            $module_count = @current_run_module_times;
            foreach my $time (@current_run_module_times) {
                $run_time += $time;
                push @module_times, $time;
            }
            push @accumulated_times, $run_time;
            foreach my $time (@current_run_reconfiguration_times) {
                $configuration_time += $time;
            }
            push @acc_reconfig_times, $configuration_time;
            foreach my $time (@current_run_data_times) {
                $data_read_time += $time;
            }
            push @acc_data_times, $data_read_time;
            foreach my $time (@current_run_data_write_times) {
                $data_write_time += $time;
            }
            push @acc_result_times, $data_write_time;
            foreach my $time (@current_run_reconfiguration_extra_times) {
                $configuration_extra_time += $time;
            }
            push @acc_reconfig_plus_times, $configuration_extra_time;
            foreach my $time (@current_init_and_run_times) {
                $init_and_run_time += $time;
            }
            push @int_and_run_times, $init_and_run_time;
        }
        print "\nAccumulated times:\n";
        print_mean_and_dev(@accumulated_times);
        print "\nInit and run times:\n";
        print_mean_and_dev(@int_and_run_times);
        print "\nData times:\n";
        print_mean_and_dev(@acc_data_times);
        #print "\nReconfiguration times:\n";
        #print_mean_and_dev(@acc_reconfig_times);
        print "\nData write times:\n";
        print_mean_and_dev(@acc_result_times);
        print "\nReconfiguration times:\n";
        print_mean_and_dev(@acc_reconfig_plus_times);
        print "\nOverall times:\n";
        print_mean_and_dev(@overall_times);

        #for(my $i = 0; $i < $module_count; $i++){
        #    my @current_module_times = ();
        #    for(my $j = 0; $j <= $#accumulated_times; $j++){
        #        push @current_module_times, @module_times[$i + $module_count * $j];
        #    }
        #    print "\nConfiguration $i:\n";
        #    print_mean_and_dev(@current_module_times);
        #}
    }
}

print getLoggingTime() . "\n";

