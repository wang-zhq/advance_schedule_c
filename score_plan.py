import sys
import os
import fnmatch
from ctypes import *

if __name__ == "__main__":
    dat_dir = r'dataset'
    res_dir = r'mysubmit'
    f_list = os.listdir(dat_dir)

    aps = cdll.LoadLibrary('./apscc.so')
    scoring = aps.resscr
    scoring.restype = c_float

    mark_all = 0
    for f in f_list:
        if fnmatch.fnmatch(f, "*.RCP"):
            dat_name = dat_dir + '/' + f
            m = scoring(dat_name.encode(), res_dir.encode())
            mark_all += m
            print('%12s: %6.2f, %8.2f'%(f, m, mark_all))
