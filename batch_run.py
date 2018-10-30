import os, sys, time
import fnmatch
import multiprocessing as mp
from ctypes import *


def ad_solve(uid):
    if fnmatch.fnmatch(uid, "*.RCP"):
        dat_dir = 'dataset'
        out_dir = 'mysubmit'
        dat_name = dat_dir + '/' + uid

        apsdl = cdll.LoadLibrary('./apscc.so')
        aps = apsdl.apsc
        aps.restype = c_float
        aps.argtypes = [c_char_p, c_char_p, c_long]

        ridx = aps(dat_name.encode(), out_dir.encode(), 20)
        return ridx
    return 0


if __name__ == "__main__":

    n_p = round(0.9*os.cpu_count())
    data_dir = 'dataset'
    unit_list = os.listdir("./%s"%(data_dir))
    pool = mp.Pool(3)
    pool.map(ad_solve, unit_list)
    pool.close()
   
    
    print("all done")