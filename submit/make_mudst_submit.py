from __future__ import print_function

from tempfile import mkstemp
from shutil import move
from os import fdopen, remove
import os
import argparse
import re
import subprocess
import time

def update_submit_script(file_path, input_file, rootdir, outdir):
  #Create temp file
  tmpfile = 'tmp_submit.xml'
  abs_path = os.path.join(os.getcwd(), tmpfile)
  with open(tmpfile,'w') as new_file:
    with open(file_path) as old_file:
      for line in old_file:
        if line.find("ENTITY INPUT") != -1 :
          new_file.write("<!ENTITY INPUT \"" + str(input_file) + "\">\n")
        elif line.find("ENTITY ROOTDIR") != -1 :
          new_file.write("<!ENTITY ROOTDIR \"" + str(rootdir) + "\">\n")
        elif line.find("ENTITY OUTDIR") != -1 :
          new_file.write("<!ENTITY OUTDIR \"" + str(outdir) + "\">\n")
        else :
          new_file.write(line)
    #Remove original file
    remove(file_path)
    #Move new file
    move(abs_path, file_path)


def main(args):
  
  ## create output directory
  out_directory = args.outputDir
  if not os.path.exists(out_directory):
    os.makedirs(out_directory)
    logdir = out_directory + '/tmplogs'
    os.makedirs(logdir)

  xml_file = os.path.join(os.getcwd(), args.submitscript)
  if not os.path.isfile(xml_file) :
    print('xmlfile doesnt exist!')
    return

  ## get our absolute path and define submission variables
  rootdir = os.getcwd()
  if os.path.isabs(out_directory):
    outdir = out_directory
  else:
    outdir = os.path.join(rootdir, out_directory)

  ## now get the list of input files
  with open(args.fileList) as f:
    filelist = f.readlines()
  
  filelist = [x.stripped() for x in filelist]

  ## and we can start job submission now
  processes = []
  for i in range(len(filelist)) :
    input_file = filelist[i]
    update_submit_script(xml_file, input_file, rootdir, outdir)

    star_submit = 'star-submit ' + xml_file
    print("submitting job: ")
    print("input file: " + input_file)
    print("output directory: " + outdir)
    print(star_submit)
    ret = subprocess.Popen(star_submit, shell=True)
    processes.append(ret)
    time.sleep(1)
    #ret.wait()
    #if ret.returncode != 0 :
      #print('warning, job submission failure')
  for i in range(len(processes)):
    proc = processes[i]
    proc.wait()
    if proc.returncode != 0:
      print('warning: job #', i, "failed submission")


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='Submit embedding jobs via star-submit')
  parser.add_argument('--fileList', default='minimc_list/piplus', help='text file listing input event.root files')
  parser.add_argument('--outputDir', default='out', help='root output directory')
  parser.add_argument('--submitscript', default='make_mudst_submit.xml', help='the xml file for star-submit')
  args = parser.parse_args()
  main( args )
