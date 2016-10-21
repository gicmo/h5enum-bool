
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

    int8_t ti = 1, fi = 0;
    filetype = H5Tenum_create (H5T_NATIVE_INT8);
    status = H5Tenum_insert (filetype, "TRUE", &ti);
    status = H5Tenum_insert (filetype, "FALSE", &fi);

    space = H5Screate(H5S_SCALAR);
    dset = H5Dcreate (file, "ds1", filetype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    fprintf(stderr, "sizeof(bool) = %lu\n", sizeof(bool));
    bool tb = true, fb = false;
    memtype = H5Tcreate (H5T_ENUM, sizeof(bool));
    status = H5Tenum_insert (memtype, "TRUE", &tb);
    status = H5Tenum_insert (memtype, "FALSE", &fb);

    bool b = true;
    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &b);

    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (filetype);
    status = H5Fclose (file);
    return 0;
}
