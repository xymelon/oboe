//
// Created by xuyangxy on 2021/4/14.
//

#ifndef SAMPLES_CACHEMODEL_H
#define SAMPLES_CACHEMODEL_H


class CacheModel {
public:
    CacheModel(float *data, int32_t size) : data(data), size(size) {}

    float *getData() {
        return data;
    }

    int32_t getSize() {
        return size;
    }

    ~CacheModel() {
        delete []data;
    }

private:
    float *data;
    int32_t size;
};


#endif //SAMPLES_CACHEMODEL_H
