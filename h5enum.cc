
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct conv_enum_t {
  size_t src_size;
  size_t dst_size;
} conv_enum_t;


static void
enum_int_converter_init (hid_t src,
			 hid_t dst,
			 H5T_cdata_t *cdata,
			 int forward)
{
  conv_enum_t *priv = (conv_enum_t *) malloc(sizeof(conv_enum_t));
  priv->src_size = H5Tget_size(src);
  priv->dst_size = H5Tget_size(dst);

  cdata->need_bkg = H5T_BKG_NO;
  cdata->priv = priv;
}

static void
enum_int_converter_free(H5T_cdata_t *cdata) {
  conv_enum_t *info = (conv_enum_t *) cdata->priv;
  free(info);
  cdata->priv = NULL;
}

static int enum_int_converter_conv(hid_t src,
				   hid_t dst,
				   H5T_cdata_t *cdata,
				   size_t nl,
				   size_t buf_stride,
				   size_t bkg_stride,
				   void *buf_i,
				   void *bkg_i,
				   hid_t dxpl,
				   int forward) {

  conv_enum_t *info = (conv_enum_t *) cdata->priv;
  hid_t supertype;
  size_t nalloc;

  char *cbuf = NULL;
  char *buf = (char *) buf_i;
  
  bool identical;
  if (forward) {
    supertype = H5Tget_super(src);
    identical = H5Tequal(supertype, dst);
  } else {
    supertype = H5Tget_super(dst);
    identical = H5Tequal(supertype, src);
  }

  if (identical) {
    return 0;
  }

  if (buf_stride == 0) {
    if (forward) {
      H5Tconvert(supertype, dst, nl, buf, NULL, dxpl);
    } else {
      H5Tconvert(src, supertype, nl, buf, NULL, dxpl);
    }
  } else {
    if (info->src_size > info->dst_size) {
      nalloc = info->src_size*nl;
    } else {
      nalloc = info->dst_size*nl;
    }

    cbuf = (char *) malloc(nalloc);
    for (int i = 0; i < nl; i++) {
      memcpy(cbuf + (i*info->src_size),
	     buf + (i*buf_stride),
	     info->src_size);
    }

    if (forward) {
      H5Tconvert(supertype, dst, nl, cbuf, NULL, dxpl);
    } else {
      H5Tconvert(src, supertype, nl, cbuf, NULL, dxpl);
    }


     for (int i = 0; i < nl; i++) {
       memcpy(buf + (i*buf_stride),
	      cbuf + (i*info->dst_size),
	      info->dst_size);
     }
  } // buf_stride == 0

  free(cbuf);
  cbuf = NULL;
  if (supertype > 0) {
    H5Tclose(supertype);
  }

  return 0;
}

static herr_t enum_int_converter(hid_t src,
				 hid_t dst,
				 H5T_cdata_t *cdata,
                    size_t nl,
				 size_t buf_stride,
				 size_t bkg_stride,
				 void *buf_i,
                               void *bkg_i,
				 hid_t dxpl,
				 int forward) {
  //Direction ("forward"): 1 = enum to int, 0 = int to enum
  static const char *direction[2] = {
    "int to enum",
    "enum to int"
  };
  
  fprintf(stderr, "enum <-> int (%s) [%d]\n", direction[forward], cdata->command);

  switch (cdata->command) {

  case H5T_CONV_INIT:
    enum_int_converter_init(src, dst, cdata, forward);
    break;

  case H5T_CONV_FREE:
    enum_int_converter_free(cdata);
    break;
  case H5T_CONV_CONV:
    return enum_int_converter_conv(src, dst, cdata, nl, buf_stride,
				   bkg_stride, buf_i, bkg_i, dxpl, forward);
  default:
    return -2;
  }

  return 0;
}

static herr_t enum2int(hid_t src_id,
		       hid_t dst_id,
		       H5T_cdata_t *cdata,
		       size_t nl,
		       size_t buf_stride,
		       size_t bkg_stride,
		       void *buf_i,
		       void *bkg_i,
		       hid_t dxpl) {
  return enum_int_converter(src_id, dst_id, cdata, nl, buf_stride, bkg_stride,
			    buf_i, bkg_i, dxpl, 1);
}

static herr_t int2enum(hid_t src_id,
		       hid_t dst_id,
		       H5T_cdata_t *cdata,
		       size_t nl,
		       size_t buf_stride,
		       size_t bkg_stride,
		       void *buf_i,
		       void *bkg_i,
		       hid_t dxpl) {
    return enum_int_converter(src_id, dst_id, cdata, nl, buf_stride, bkg_stride,
			      buf_i, bkg_i, dxpl, 0);
}

typedef struct {
  bool b;
  const char *s;
} value_t;

int
main (void)
{
    hid_t       file, filetype, memtype, space, dset;
    herr_t      status;
    hsize_t     dims[1] = {1};
    int         ndims,
                i, j;

    hid_t tenum = H5Tenum_create(H5T_STD_I32LE);
    H5Tregister(H5T_PERS_SOFT, "enum2int", tenum, H5T_STD_I32LE, enum2int);
    H5Tregister(H5T_PERS_SOFT, "int2enum", H5T_STD_I32LE, tenum, int2enum);

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

    fprintf(stderr, "-- bool -- \n");
    bool b = true;
    status = H5Dwrite (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &b);

    fprintf(stderr, "-- int -- \n");
    int foo = 0;
    status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &foo);

    fprintf(stderr, "-- int8 -- \n");
    int8_t bar = 1;
    status = H5Dwrite (dset, H5T_NATIVE_INT8, H5S_ALL, H5S_ALL, H5P_DEFAULT, &bar);

    hid_t ft2 = H5Tcopy(H5T_C_S1);
    H5Tset_size(ft2, H5T_VARIABLE);

    fprintf(stderr, "-- str -- \n");

    space = H5Screate(H5S_SCALAR);
    hid_t ds2 = H5Dcreate (file, "ds2", ft2, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    const char *tchar[1] = {"Hallo Welt"};
    hid_t mt = H5Tcopy(H5T_C_S1);
    H5Tset_size(mt, strlen(tchar[0]));

    status = H5Dwrite (ds2, ft2, H5S_ALL, H5S_ALL, H5P_DEFAULT, tchar);


    //
    fprintf(stderr, "-- struct {bool, string} -- \n");
    fprintf(stderr, "sizeof (value_t) = %lu [b @ %lu, s @ %lu]\n",
	    sizeof (value_t), HOFFSET (value_t, b), HOFFSET (value_t, s));
    fprintf(stderr, "sizeof (bool) = %lu, sizeof (char *) = %lu, SUM: %lu\n",
	    sizeof (bool), sizeof (char *), sizeof (bool) + sizeof (char *));

    hid_t ctm = H5Tcreate (H5T_COMPOUND, sizeof (value_t));
    status = H5Tinsert (ctm, "bool", HOFFSET (value_t, b), memtype);
    status = H5Tinsert (ctm, "string", HOFFSET (value_t, s), ft2);

    hid_t ctf = H5Tcreate (H5T_COMPOUND, sizeof (value_t));
    status = H5Tinsert (ctf, "bool", HOFFSET (value_t, b), filetype);
    status = H5Tinsert (ctf, "string", HOFFSET (value_t, s), ft2);

    const value_t vals[1] = {{true, "hallo welt"}};

    hid_t ds3 = H5Dcreate (file, "ds3", ctf, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (ds3, ctm, H5S_ALL, H5S_ALL, H5P_DEFAULT, vals);

    fprintf(stderr, "-- ┬─┬ノ( º _ ºノ)  --\n");

    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (filetype);
    status = H5Fclose (file);
    return 0;
}
