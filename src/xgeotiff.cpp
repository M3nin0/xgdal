#include <iostream>

#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"
#include "gdal/gdalwarper.h"

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"
#include "xtensor/xview.hpp"

#include "xgeotiff.hpp"


template<typename T>
xg::XGeoTiff<T>::XGeoTiff(const char *filename)
{
    GDALAllRegister();

    this->geotiffDataset = (GDALDataset*) GDALOpen(filename, GA_ReadOnly);

    this->nrow = GDALGetRasterYSize(this->geotiffDataset);
    this->ncol = GDALGetRasterXSize(this->geotiffDataset);
}

template<typename T>
xg::XGeoTiff<T>::~XGeoTiff()
{
    GDALClose(this->geotiffDataset);
    GDALDestroyDriverManager();
}

template<typename T>
const xt::xarray<T> xg::XGeoTiff<T>::GetArray(int band)
{
    GDALRasterBand *rasterBand = this->geotiffDataset->GetRasterBand(band);
    GDALDataType bandType = GDALGetRasterDataType(rasterBand);

    int nBytesBandType = GDALGetDataTypeSizeBytes(bandType);
    T * buffer = (T*) CPLMalloc(nBytesBandType * this->ncol);

    xt::xarray<T> imageData = xt::zeros<T>({this->nrow, this->ncol, 1});

    for(int row = 0; row < this->nrow; row++)
    {
        CPLErr rasterIOError = rasterBand->RasterIO(
            GF_Read, 0, row, ncol, 1, buffer, ncol, 1, bandType, 0, 0
        );

        if (!(rasterIOError == 0))
        {
            throw std::runtime_error("GDAL Error. Read file problem");
        }

        for(int col = 0; col < this->ncol; col++)
        {
            imageData(row, col, 1) = (T) buffer[col];
        }
    }
    return imageData;
}

template<typename T>
const char* xg::XGeoTiff<T>::GetProjection()
{
    return this->geotiffDataset->GetProjectionRef();
}

template<typename T>
const double* xg::XGeoTiff<T>::GetGeoTransform()
{
    this->geotiffDataset->GetGeoTransform(this->geotransform);
    return this->geotransform;
}

template<typename T>
const int* xg::XGeoTiff<T>::Shape()
{
    int *shape = new int[2];

    shape[0] = this->nrow;
    shape[1] = this->ncol;

    return shape;
}
