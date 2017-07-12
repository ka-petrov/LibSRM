#include "srm.h"

#include <iostream>
#include <cmath>

// using std::fill;

float g = 256; // number of different intensity values
float delta;

/*
 * The predicate: is the difference of the averages of the two
 * regions R and R' smaller than
 *
 * g sqrt(1/2Q (1/|R| + 1/|R'|) ln 2/delta)
 *
 * Instead of calculating the square root all the time, we calculate
 * the factor g^2 / 2Q ln 2/delta, and compare
 *
 * (<R> - <R'>)^2 < factor (1/|R| + 1/|R'|)
 *
 * instead.
 */
float factor, logDelta;

/*
 * For performance reasons, these are held in w * h arrays
 */
float* average;
int* count;
int* regionIndex; // if < 0, it is -1 - actual_regionIndex
int im_w, im_h;
/*
 * The statistical region merging wants to merge regions in a specific
 * order: for all neighboring pixel pairs, in ascending order of
 * intensity differences.
 *
 * In that order, it is tested if the regions these two pixels belong
 * to (by construction, the regions must be distinct) should be merged.
 *
 * For efficiency, we do it by bucket sorting, because there are only
 * g + 1 possible differences.
 *
 * After sorting, for each difference the pixel pair with the largest
 * index is stored in neighborBuckets[difference], and every
 * nextNeighbor[index] points to the pixel pair with the same
 * difference and the next smaller index (or -1 if there is none).
 *
 * The pixel pairs are identified by
 *
 *     2 * (x + (w - 1) * y) + direction
 *
 * where direction = 0 means "right neighbor", and direction = 1 means
 * " lower neighbor".  (We do not need "left" or "up", as the order
 * within the pair is not important.)
 *
 * In n dimensions, it must be n * pixel_count, and "direction"
 * specifies the Cartesian unit vector (axis) determining the neighbor.
 */
int* nextNeighbor;
int* neighborBucket;


void initRegions(const uchar *imdata, int w, int h) {
    int len = w*h;
    average = new float[len];
    count = new int[len];
    regionIndex = new int[len];

    for (int i=0; i < len; i++) {
        // average[i] = imdata[i] & 0xff;
        average[i] = imdata[i];
        count[i] = 1;
        regionIndex[i] = i;
    }
}

void addNeighborPair(int neighborIdx, const uchar* imdata, int i1, int i2) {
    // int difference = abs((imdata[i1] & 0xff) - (imdata[i2] & 0xff));
    int difference = abs(imdata[i1] - imdata[i2]);
    nextNeighbor[neighborIdx] = neighborBucket[difference];
    neighborBucket[difference] = neighborIdx;
}

void initNeighbors(const uchar *imdata, int w, int h) {
    nextNeighbor = new int[2*w*h];

    // bucket sort
    // [g] was hardcoded here as 256 in the original implementation
    int len = (int)g;
    neighborBucket = new int[len];
    // this could be replaced by std function
    for (int i=0; i < len; i++)
        neighborBucket[i] = -1;

    for (int j=h-1; j >= 0; j--)
        for (int i=w-1; i >= 0; i--) {
            int idx = i + w * j;
            int neighborIdx = 2*idx;

            // vertical
            if (j < h-1)
                addNeighborPair(neighborIdx+1, imdata, idx, idx+w);
            // horizontal
            if (i < w-1)
                addNeighborPair(neighborIdx, imdata, idx, idx+1);
        }
}

// recursively find out the region index for this pixel
int getRegionIndex(int i) {
    i = regionIndex[i];
    while (i < 0)
        i = regionIndex[-1 - i];
    return i;
}

// should regions i1 and i2 be merged?
bool predicate(int i1, int i2) {
    float difference = average[i1] - average[i2];
    /*
     * This would be the non-relaxed predicate mentioned in the
     * paper.
     *
     * return difference * difference < factor * (1f / count[i1] + 1f / count[i2]);
     *
     */
    float log1 = logf(1 + count[i1]) * (g < count[i1] ? g : count[i1]);
    float log2 = logf(1 + count[i2]) * (g < count[i2] ? g : count[i2]);
    return difference * difference <
           .1f * factor * ((log1 + logDelta) / count[i1]
                           + ((log2 + logDelta) / count[i2]));
}

void mergeRegions(int i1, int i2) {
    if (i1 == i2)
        return;
    int mergedCount = count[i1] + count[i2];
    float mergedAverage = (average[i1] * count[i1]
                           + average[i2] * count[i2]) / mergedCount;

    // merge larger index into smaller index
    if (i1 > i2) {
        average[i2] = mergedAverage;
        count[i2] = mergedCount;
        regionIndex[i1] = -1 - i2;
    }
    else {
        average[i1] = mergedAverage;
        count[i1] = mergedCount;
        regionIndex[i2] = -1 - i1;
    }
}

void mergeAllNeighbors(int w) {
    int len = (int)g;
    for (int i = 0; i < len; i++) {
        int neighborIndex = neighborBucket[i];
        while (neighborIndex >= 0) {
            int i1 = neighborIndex / 2;
            int i2 = i1 + (0 == (neighborIndex & 1) ? 1 : w);

            i1 = getRegionIndex(i1);
            i2 = getRegionIndex(i2);

            if (predicate(i1, i2))
                mergeRegions(i1, i2);

            neighborIndex = nextNeighbor[neighborIndex];
        }
    }
}

int consolidateRegions() {
    /*
     * By construction, a negative regionIndex will always point
     * to a smaller regionIndex.
     *
     * So we can get away by iterating from small to large and
     * replacing the positive ones with running numbers, and the
     * negative ones by the ones they are pointing to (that are
     * now guaranteed to contain a non-negative index).
     */
    int len = im_w*im_h;
    int count = 0;
    for (int i = 0; i < len; i++)
        if (regionIndex[i] < 0)
            regionIndex[i] = regionIndex[-1 - regionIndex[i]];
        else
            regionIndex[i] = count++;
    return count;
}

void runSrm(float Q, const uchar* imdata, int w, int h,
            bool showAvg, bool showLbl,
            float* avgOut, int* lblOut) {
    im_w = w;
    im_h = h;
    delta = 1.0f / (6 * w * h);
    /*
     * This would be the non-relaxed formula:
     * factor = g * g / 2 / Q * (float)Math.log(2 / delta);
     * The paper claims that this is more prone to oversegmenting.
     */
    factor = g * g / 2.0f / Q;
    logDelta = 2.0f * logf(6.0f * w * h);

    initRegions(imdata, w, h);
    initNeighbors(imdata, w, h);
    mergeAllNeighbors(w);

    int len = w*h;
    if (showAvg) {
        // [avgOut] should be allocated by caller!
        // avgOut = new float[len];
        for (int i = 0; i < len; i++) {
            average[i] = average[getRegionIndex(i)];
            avgOut[i] = average[i];
        }
    }
    if (showLbl) {
        int regionCount = consolidateRegions();
        // [lblOut] should be allocated by caller!
        // lblOut = new int[len];
        for (int i=0; i < len; i++)
            lblOut[i] = regionIndex[i];
    }
}