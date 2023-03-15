#! /usr/bin/env python3

import os
import time
from subprocess import Popen, PIPE, check_output, STDOUT, TimeoutExpired
from sys import exit
from optparse import OptionParser
from shlex import split

################################################################################################################

PROGRAM_NAME = "./VertexCover"  # Program to execute
CSV_FILE = ""  # CSV Path, leave empty for no csv output
MULTIPLE_CSV = True  # Enables individual CSV for each directory
MAXSECPERGRAPH = 2.5  # allowed time (in seconds) for each instance
MAXSECTOTAL = 43200  # Total max running time (soft stop: no further program execution started after this time)
MAXNOTSOLVED = 10  # count of instances the program is allowed to timeout till death (next folder)
DIRECTORY = "./res/"  # directory the other folders (with files and configs) are in
CONFIG = ""  # passes config to program via -c
PRE_CONFIG = ""  # passes pre-config to program via -p
OPTIONS = ""  # Custom commandline-options for executable program
LOG = "" # Provide a logfile and the programs comments (lines beginning with #) are written into it

################################################################################################################


usage = "Usage: ./benchmark.py [options]"
parser = OptionParser(usage=usage)

parser.add_option("-x", "--program",
                  action="store", type="string", dest="program", default=PROGRAM_NAME, help="program to execute")
parser.add_option("--csv",
                  action="store", type="string", dest="csv", default=CSV_FILE, help="CSV output file")
parser.add_option("-n", "--nomcsv", "--no_multiple_csv", "--no-multiple-csv",
                  action="store_false", dest="mcsv", default=MULTIPLE_CSV,
                  help="This option disables individual CSV output for each folder.")
parser.add_option("-s", "--maxseconds", "--ms",
                  action="store", type="float", dest="maxsec", default=MAXSECPERGRAPH,
                  help="allowed time (in seconds) for each instance")
parser.add_option("-t", "--totalmaxseconds", "--tms", "--totalseconds",
                  action="store", type="float", dest="tmaxsec", default=MAXSECTOTAL,
                  help="allowed total time (in seconds) for all instances")
parser.add_option("-i", "--maxinstances", "--mi",
                  action="store", type="int", dest="maxins", default=MAXNOTSOLVED,
                  help="count of instances the program is allowed to timeout within each directory")
parser.add_option("-d", "--dir", "--directory",
                  action="store", type="string", dest="dir", default=DIRECTORY,
                  help="path to the directory where folders with instances are in,\n "
                       "i.e. the directory in which the \'snap\', \'random\' and \'dimacs\' folders are located")
parser.add_option("-j", "-c", "--conf", "--config", "--json",
                  action="store", dest="config", default=CONFIG,
                  help="This option passes a config to program via '-c'")
parser.add_option("-p", "--pre", "--pre-config", "--pre-json",
                  action="store", dest="pre_config", default=PRE_CONFIG,
                  help="This option passes a pre-config to program via '-p'")
parser.add_option("-l", "--log", "--logfile", "--log-file",
                  action="store", dest="log", default=LOG,
                  help="Provide a logfile and the programs comments (lines beginning with #) are written into it")
parser.add_option("-o", "--options", "--program-options",
                  action="store", type="string", dest="opts", default=OPTIONS,
                  help="Custom command-line options for program, please submit as a string")


(options, args) = parser.parse_args()

PROGRAM_NAME = options.program + " " + options.opts
CSV_FILE = options.csv
MULTIPLE_CSV = options.mcsv
MAXSECPERGRAPH = options.maxsec
MAXSECTOTAL = options.tmaxsec
MAXNOTSOLVED = options.maxins
DIRECTORY = options.dir
CONFIG = options.config
PRE_CONFIG = options.pre_config
LOG = options.log

if CONFIG != "":
    PROGRAM_NAME += " -p " + PRE_CONFIG
if CONFIG != "":
    PROGRAM_NAME += " -c " + CONFIG

################################################################################################################


def pretty_print_line(exec_time, our_solution, exact_solution, recursive_steps):
    print("{:>10.1f}\t\t".format(exec_time), end='', flush=True)
    print("{:<12}\t".format(str(our_solution)), end='', flush=True)
    print("{:<12}\t".format(str(exact_solution)), end='', flush=True)

    if exec_time == MAXSECPERGRAPH:
        print("{:^15}\t".format("ABORTED"), end='', flush=True)
    else:
        print("{:^15}\t".format("finished"), end='', flush=True)
        global count_finished
        count_finished += 1

    if our_solution is not None and exact_solution is not None:
        if our_solution != exact_solution:
            print("{:^15}\t".format(">>INCORRECT<<"), end='', flush=True)
            global count_incorrect
            count_incorrect += 1
        else:
            print("{:^15}\t".format("correct"), end='', flush=True)
            global count_correct
            count_correct += 1

    if recursive_steps is not None:
        print(recursive_steps, end='', flush=True)
    print("", flush=True)


def csv_header():
    with open(CSV_FILE, 'w') as csv:
        csv.write("Filename,")
        csv.write("Execution Time,")
        csv.write("Found VC,")
        csv.write("Solution VC,")
        csv.write("Finished?,")
        csv.write("Correct?,")
        csv.write("Recursive Steps,")
        csv.write("Vertices,")
        csv.write('\n')


def csv_write(file, exec_time, our_solution, exact_solution, recursive_steps, vertices):
    with open(CSV_FILE, 'a') as csv:
        csv.write(file.rsplit(sep='/', maxsplit=1)[1] + ',')
        csv.write(str(exec_time) + ',')
        csv.write(str(our_solution) + ',')
        csv.write(str(exact_solution) + ',')
        csv.write(str(exec_time != MAXSECPERGRAPH) + ',')
        csv.write(str(our_solution is not None and exact_solution is not None and our_solution == exact_solution) + ',')
        if recursive_steps is not None:
            csv.write(str(recursive_steps) + ',')
        else:
            csv.write("None,")
        if vertices is not None:
            csv.write(str(vertices))
        else:
            csv.write("None")
        csv.write('\n')


def log_write(line):
    if LOG != "":
        with open(LOG, 'a') as log:
            log.write(line)
            log.write('\n')


def get_solution_from_output(str):
    rec = None  # None if no recursive steps
    vertices = None
    sol = 0
    for line in str.splitlines():
        if line.startswith("#") or line.startswith("c "):       # we are doomed if a vertex is named "c" :)
            log_write(line)
            if "recursive" in line or "Recursive" in line or "RECURSIVE" in line:
                rec = int(line.rsplit(maxsplit=1)[1])
            elif "vertices" in line or "Vertices" in line or "VERTICES" in line:
                vertices = int(line.rsplit(maxsplit=1)[1])
            else:
                continue
        else:
            sol += 1
    return sol, rec, vertices


def shoot_once(file):
    if file.endswith(".solution") or file.endswith("Summary"):  # no graph, otherwise it is treated as a graph
        return True

    print("{:<40}\t".format(file.rsplit(sep='/', maxsplit=1)[1] + ':'), end="\t", flush=True)  # first column printed
    global count_jobs
    count_jobs += 1
    solved = True

    with open(file, "rb") as f:  # getting input data
        graph = f.read()
    prog_output = b''
    global p
    args = split(PROGRAM_NAME)
    start_time = time.time()
    try:
        p = Popen(args, stdout=PIPE, stdin=PIPE, stderr=STDOUT)  # starting program
        prog_output = p.communicate(input=graph, timeout=MAXSECPERGRAPH)[0]
    except TimeoutExpired:
        pass
    except KeyboardInterrupt:
        print("\n\nSignal received, shutting down\n", flush=True)
        exit(1)
    finally:
        p.kill()

    execution_time = time.time() - start_time
    if execution_time > MAXSECPERGRAPH:
        execution_time = float(MAXSECPERGRAPH)

    prog_output = prog_output.decode("utf-8")
    solution_file = file.rsplit(sep=".", maxsplit=1)[0] + ".solution"  # corresponding solution file

    optimal_solution = None  # exact solution value
    if os.path.isfile(solution_file):
        with open(solution_file) as sol:
            str = sol.readline()
            if not str.startswith("Time"):
                optimal_solution = int(str)

    (our_solution, recursive_steps, vertices) = get_solution_from_output(prog_output)
    if execution_time == MAXSECPERGRAPH:
        our_solution = None
        solved = False
    pretty_print_line(execution_time, our_solution, optimal_solution, recursive_steps)
    if CSV_FILE != "":
        csv_write(file, execution_time, our_solution, optimal_solution, recursive_steps, vertices)
    return solved


################################################################################################################

START_TIME = time.time()

count_jobs = 0
count_finished = 0
count_correct = 0
count_incorrect = 0

dir_list = [x for x in os.listdir(DIRECTORY) if
            not (not os.path.isdir(DIRECTORY + x) or x.startswith('.') or x.startswith("config"))]
if len(dir_list) <= 0:
    print("NO DIRECTORIES!")
    exit(1)

# header line
print("{:<42}\t\t{:^20}{:^12}{:^18}\t{:^14}\t{:^14}\t{:^14}"
      .format("Filename", "ExecTime", "FoundVC", "RealSize", "Status", "Correctness", "Recursive Steps"),
      flush=True)
if CSV_FILE != "" and not MULTIPLE_CSV:
    csv_header()

csv_save = CSV_FILE
for directory in dir_list:
    print("\n\tENTERING FOLDER: " + directory + "\n\n", flush=True)
    #log_write("\n\tENTERING FOLDER: " + directory + "\n")
    if MULTIPLE_CSV and CSV_FILE != "":
        if "." in csv_save:
            CSV_FILE = csv_save.rsplit(sep='.', maxsplit=1)[0] + "_" + directory + "." + \
                       csv_save.rsplit(sep='.', maxsplit=1)[1]
        else:
            CSV_FILE = csv_save + "_" + directory
        csv_header()

    unsolved_instances = 0

    contents = os.listdir(DIRECTORY + directory)
    contents = [DIRECTORY + directory + "/" + x for x in contents if not x.endswith(".solution")]
    contents = sorted(contents, key=os.path.getsize)

    for f in contents:
        # if os.path.getsize(DIRECTORY + directory + "/" + f) < 100:
        if time.time() - START_TIME >= MAXSECTOTAL:
            break

        log_write('\n' + f)
        if not shoot_once(f):
            unsolved_instances += 1
        if unsolved_instances == MAXNOTSOLVED:
            print("\tTimeouted " + str(MAXNOTSOLVED) + " instances in this directory. Entering next directory.",
                  flush=True)
            break

    if time.time() - START_TIME >= MAXSECTOTAL:
        print("Total time of " + str(MAXSECTOTAL) + " over. Shutting down.", flush=True)
        break

print()
print("Summary of config: Of {} jobs were {} finished, {} solved correctly and {} solved incorrectly"
      .format(count_jobs, count_finished, count_correct, count_incorrect), flush=True)

