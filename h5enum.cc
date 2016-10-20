
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>

int
main (void)
{
    hid_t       file, filetype, memtype, space, dset;
    herr_t      status;
    hsize_t     dims[1] = {1};
    int         ndims,
                i, j;

    file = H5Fcreate ("h5enum.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    int8_t t = 1, f = 0;
    filetype = H5Tenum_create (H5T_NATIVE_INT8);
    status = H5Tenum_insert (filetype, "TRUE", &t);
    status = H5Tenum_insert (filetype, "FALSE", &f);

    space = H5Screate_simple (1, dims, NULL);
    dset = H5Dcreate (file, "ds1", filetype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    bool b = true;
    fprintf(stderr, "sizeof(bool) = %lu\n", sizeof(b));
    memtype = H5Tcopy(H5T_NATIVE_INT8);
    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &b);

    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (filetype);
    status = H5Fclose (file);
    return 0;
}
