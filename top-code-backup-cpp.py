#!/usr/bin/env python3

import datetime
import os
import re
import shutil
import subprocess
import sys

ARCHIVE_NAME = 'Utf16ToAscii'
BACKUP_DIR = 'C:/Users/uderos/backup'

BACKUP_EXTENSION_LIST = [ 'h', 'hpp', 'c', 'cpp', 'ini', 'rc' ]

IGNORE_DIR_LIST = [ 
	'/Debug/', '/Release/', '/.vs/', '/ipch/', '/_Bin/', '/ReleaseNotes/' ]

def shell_exec(cmd, exp_rc = 0):
	print("ShellExec: {}".format(cmd))
	rc = subprocess.run(cmd).returncode
	if not exp_rc is None and rc != exp_rc:
		errmsg = "ShellExec FAILURE rc={}/{}".format(rc, exp_rc)
		raise RuntimeError(errmsg)
	return rc


#def is_binary_file(fname):
#	"""Return true if the given fname is binary.
#	@raise EnvironmentError: if the file does not exist or cannot be accessed.
#	@attention: found @ http://bytes.com/topic/python/answers/21222-determine-file-type-binary-text on 6/08/2010
#	@author: Trent Mick <TrentM@ActiveState.com>
#	@author: Jorge Orpinel <jorge@orpinel.com>"""
##	try:
#	with open(fname, 'rb') as fin:
#		CHUNKSIZE = 1024
#		while 1:
#			chunk = fin.read(CHUNKSIZE)
#			if b'\x00' in chunk: # found null byte
#				return True
#			if len(chunk) < CHUNKSIZE:
#				break # done
##	except Exception as err:
##		print("EXCEPTION: <<{}>> reading {}".format(err, fname))
#	return False

def get_file_extension(fname):
	extension = ""
	try:
		basename = str(os.path.basename(fname))
		tmp = str(basename).split('.')
		if len(tmp) > 1:
			extension =  tmp[-1]
	except Exception as err:
		pass
	return extension

def convert_abs_path(abs_path):
	result = ''
	path_list = []
	head = abs_path
	tail = 'XXXX'
	while len(tail) > 0:
		(head, tail) = os.path.split(head)
		path_list.append(tail)

	path_list.reverse();
	for p in path_list:
		result = os.path.join(result, p)

	return result


def is_backup_required_for_file(fname_abs):
	fpath = convert_abs_path(fname_abs)
	basename = str(os.path.basename(fname_abs))
	extension = get_file_extension(fname_abs)
			
	for d in IGNORE_DIR_LIST:
		if d in fname_abs.replace("\\", "/"):
			return False
	for e in BACKUP_EXTENSION_LIST:
		if e == extension:
			return True

	return False;

def generate_file_list(root_dir):
	backup_file_list = []
	if not os.path.isdir(root_dir):
		raise RuntimeError("Root directory {} not found".format(root_dir))
	for dir_name, subdirs, files in os.walk(root_dir):
		for fname in files:
			fname_abs = os.path.join(dir_name, fname)
			if is_backup_required_for_file(fname_abs):
				backup_file_list.append(fname_abs)
	return backup_file_list
			
def create_timestamp():
	now = datetime.datetime.now()
	#timestamp = "Timestamp: {%Y%m%d%H%M%S}".format(now)
	timestamp = now.strftime('%Y%m%d%H%M%S')
	return timestamp

def create_file_list_file(file_list, fname):
	with open(fname, 'w') as f:
		for elem in file_list:
			f.write("\"{}\"\n".format(elem))


def create_archive(file_list):
	tempfile = "{}/backup-file-list.txt".format(os.environ['TEMP'])
	create_file_list_file(file_list, tempfile)
	timestamp = create_timestamp()
	archive_fname = "{}-{}.7z".format(ARCHIVE_NAME, timestamp)
	cmd = "7z a -spf2 -scsWIN {} @{}".format(archive_fname, tempfile)
	shell_exec(cmd)
	return archive_fname

def backup_archive(archive_abs_fname):
	archive_basename = os.path.basename(archive_abs_fname)
	destination_abs_name = "{}/{}".format(BACKUP_DIR, archive_basename)
	shutil.copy(archive_abs_fname, destination_abs_name)
	print("Backup file: {}".format(destination_abs_name))

def main():
	root_dir = os.getcwd() + "/Utf16ToAscii"
	file_list = generate_file_list(root_dir)
	archive_abs_fname = create_archive(file_list)
	backup_archive(archive_abs_fname)
	
if __name__ == '__main__':
    main()


