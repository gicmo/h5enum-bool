#include <hdf5.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// compile:
// h5c++ -o h5crash h5crash.cc
//
// # crash it with h5py 2.6.0:
// import h5py as h5
// fd = h5.File('h5pycrash.h5')
// fd['ds'][()]
// -> segmentation fault


typedef struct {
  bool b;
  char *s;
} value_t;

int
main (void)
{
   int8_t ti = 1, fi = 0;
   hid_t filetype = H5Tenum_create (H5T_NATIVE_INT8);
   H5Tenum_insert (filetype, "TRUE", &ti);
   H5Tenum_insert (filetype, "FALSE", &fi);

   bool tb = true, fb = false;
   hid_t memtype = H5Tcreate (H5T_ENUM, sizeof(bool));
   H5Tenum_insert (memtype, "TRUE", &tb);
   H5Tenum_insert (memtype, "FALSE", &fb);

   hid_t file = H5Fcreate ("h5pycrash.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   hid_t strtype = H5Tcopy(H5T_C_S1);
   H5Tset_size(strtype, H5T_VARIABLE);

   hid_t ctm = H5Tcreate (H5T_COMPOUND, sizeof (value_t));
   H5Tinsert (ctm, "bool", HOFFSET (value_t, b), memtype);
   H5Tinsert (ctm, "string", HOFFSET (value_t, s), strtype);

   hid_t ctf = H5Tcreate (H5T_COMPOUND, sizeof (value_t));
   H5Tinsert (ctf, "bool", HOFFSET (value_t, b), filetype);
   H5Tinsert (ctf, "string", HOFFSET (value_t, s), strtype);

   char *str = strdup("hallo welt");
   value_t vals[1] = {{true, str}};

   hid_t space = H5Screate(H5S_SCALAR);
   hid_t ds = H5Dcreate (file, "ds", ctf, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
   H5Dwrite (ds, ctm, H5S_ALL, H5S_ALL, H5P_DEFAULT, vals);

   H5Sclose (space);
   H5Tclose (filetype);
   H5Tclose (memtype);
   H5Tclose (ctm);
   H5Tclose (ctf);
   H5Dclose (ds);
   H5Fclose (file);

   free (str);

   fprintf(stderr, "(╯°□°）╯︵ ┻━┻\n");
   return 0;
}
