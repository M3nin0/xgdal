namespace xg {
    template<typename T>
    class XGeoTiff
    {
    private:
        int nrow;
        int ncol;
        
        double geotransform[6];
        GDALDataset *geotiffDataset;
    public:
        XGeoTiff(const char* filename);
        ~XGeoTiff();

        const xt::xarray<T> GetArray(int band);

        const int *Shape();
        const char *GetProjection();
        const double *GetGeoTransform();
    };
}
