#include <corem/module/gaussFilter.h>
#include <cassert>

GaussFilter::GaussFilter(std::string id, retina_config_t *conf, double sigma)
    : Module(id, conf) {
    // transform sigma to pixels
    sigma_ = sigma * conf->pixelsPerDegree;

    // TODO: why 0.1?
    inputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);

    // reserve space for all possible threads
    buffer_ = new double[(columns + rows) * omp_get_max_threads()];

    // coefficient calculation
    q_ = 0.98711 * sigma_ - 0.96330;

    if (sigma_ < 2.5)
        q_ = 3.97156 - 4.14554 * std::sqrt(1.0 - 0.26891 * sigma_);

    b0_ = 1.57825 + 2.44413 * q_ + 1.4281 * q_ * q_ + 0.422205 * q_ * q_ * q_;
    b1_ = 2.44413 * q_ + 2.85619 * q_ * q_ + 1.26661 * q_ * q_ * q_;
    b2_ = -1.4281 * q_ * q_ - 1.26661 * q_ * q_ * q_;
    b3_ = 0.422205 * q_ * q_ * q_;
    B_ = 1.0 - (b1_ + b2_ + b3_) / b0_;

    b1_ /= b0_;
    b2_ /= b0_;
    b3_ /= b0_;

    // From: Bill Triggs, Michael Sdika: Boundary Conditions for Young-van
    // Vliet
    // Recursive Filtering
    M_[0][0] = -b3_ * b1_ + 1.0 - b3_ * b3_ - b2_;
    M_[0][1] = (b3_ + b1_) * (b2_ + b3_ * b1_);
    M_[0][2] = b3_ * (b1_ + b3_ * b2_);
    M_[1][0] = b1_ + b3_ * b2_;
    M_[1][1] = -(b2_ - 1.0) * (b2_ + b3_ * b1_);
    M_[1][2] = -(b3_ * b1_ + b3_ * b3_ + b2_ - 1.0) * b3_;
    M_[2][0] = b3_ * b1_ + b2_ + b1_ * b1_ - b2_ * b2_;
    M_[2][1] = b1_ * b2_ + b3_ * b2_ * b2_ - b1_ * b3_ * b3_ - b3_ * b3_ * b3_ -
               b3_ * b2_ + b3_;
    M_[2][2] = b3_ * (b1_ + b3_ * b2_);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            // A factor of (1.0-b1_-b2_-b3_) seems to be missing (check the
            // publication cited above)
            M_[i][j] /=
                (1.0 + b1_ - b2_ + b3_) * (1.0 + b2_ + (b1_ - b3_) * b3_);
}

GaussFilter::~GaussFilter() {
    delete inputImage_;
    delete outputImage_;
    delete[] buffer_;
}

//------------------------------------------------------------------------------//

void GaussFilter::gaussVertical(CImg<double> *src) {
    //#pragma omp parallel for num_threads(4)

    for (int i = 0; i < columns; i++) {
        double *temp2 = buffer_ + omp_get_thread_num() * (columns + rows);

        temp2[0] =
            B_ * (double)(*src)(i, 0, 0) + b1_ * (double)(*src)(i, 0, 0) +
            b2_ * (double)(*src)(i, 0, 0) + b3_ * (double)(*src)(i, 0, 0);

        temp2[1] = B_ * (double)(*src)(i, 1, 0) + b1_ * temp2[0] +
                   b2_ * (double)(*src)(i, 0, 0) +
                   b3_ * (double)(*src)(i, 0, 0);

        temp2[2] = B_ * (double)(*src)(i, 2, 0) + b1_ * temp2[1] +
                   b2_ * temp2[0] + b3_ * (double)(*src)(i, 0, 0);

        for (int j = 3; j < rows; j++)
            temp2[j] = B_ * (double)(*src)(i, j, 0) + b1_ * temp2[j - 1] +
                       b2_ * temp2[j - 2] + b3_ * temp2[j - 3];

        double temp2Wm1 =
            (double)(*src)(i, rows - 1, 0) +
            M_[0][0] * (temp2[rows - 1] - (double)(*src)(i, rows - 1, 0)) +
            M_[0][1] * (temp2[rows - 2] - (double)(*src)(i, rows - 1, 0)) +
            M_[0][2] * (temp2[rows - 3] - (double)(*src)(i, rows - 1, 0));

        double temp2W =
            (double)(*src)(i, rows - 1, 0) +
            M_[1][0] * (temp2[rows - 1] - (double)(*src)(i, rows - 1, 0)) +
            M_[1][1] * (temp2[rows - 2] - (double)(*src)(i, rows - 1, 0)) +
            M_[1][2] * (temp2[rows - 3] - (double)(*src)(i, rows - 1, 0));

        double temp2Wp1 =
            (double)(*src)(i, rows - 1, 0) +
            M_[2][0] * (temp2[rows - 1] - (double)(*src)(i, rows - 1, 0)) +
            M_[2][1] * (temp2[rows - 2] - (double)(*src)(i, rows - 1, 0)) +
            M_[2][2] * (temp2[rows - 3] - (double)(*src)(i, rows - 1, 0));

        temp2[rows - 1] = temp2Wm1;
        temp2[rows - 2] = B_ * temp2[rows - 2] + b1_ * temp2[rows - 1] +
                          b2_ * temp2W + b3_ * temp2Wp1;

        temp2[rows - 3] = B_ * temp2[rows - 3] + b1_ * temp2[rows - 2] +
                          b2_ * temp2[rows - 1] + b3_ * temp2W;

        for (int j = rows - 4; j != -1; j--)
            temp2[j] = B_ * temp2[j] + b1_ * temp2[j + 1] + b2_ * temp2[j + 2] +
                       b3_ * temp2[j + 3];

        for (int j = 0; j < rows; j++) (*src)(i, j, 0) = (double)temp2[j];
    }
}

void GaussFilter::gaussHorizontal(CImg<double> *src) {
    //#pragma omp parallel for num_threads(4)

    for (int i = 0; i < rows; i++) {
        double *temp2 = buffer_ + omp_get_thread_num() * (rows + columns);

        temp2[0] =
            B_ * (double)(*src)(0, i, 0) + b1_ * (double)(*src)(0, i, 0) +
            b2_ * (double)(*src)(0, i, 0) + b3_ * (double)(*src)(0, i, 0);

        temp2[1] = B_ * (double)(*src)(1, i, 0) + b1_ * temp2[0] +
                   b2_ * (double)(*src)(0, i, 0) +
                   b3_ * (double)(*src)(0, i, 0);

        temp2[2] = B_ * (double)(*src)(2, i, 0) + b1_ * temp2[1] +
                   b2_ * temp2[0] + b3_ * (double)(*src)(0, i, 0);

        for (int j = 3; j < columns; j++)
            temp2[j] = B_ * (double)(*src)(j, i, 0) + b1_ * temp2[j - 1] +
                       b2_ * temp2[j - 2] + b3_ * temp2[j - 3];

        double temp2Wm1 =
            (double)(*src)(columns - 1, i, 0) +
            M_[0][0] *
                (temp2[columns - 1] - (double)(*src)(columns - 1, i, 0)) +
            M_[0][1] *
                (temp2[columns - 2] - (double)(*src)(columns - 1, i, 0)) +
            M_[0][2] * (temp2[columns - 3] - (double)(*src)(columns - 1, i, 0));

        double temp2W =
            (double)(*src)(columns - 1, i, 0) +
            M_[1][0] *
                (temp2[columns - 1] - (double)(*src)(columns - 1, i, 0)) +
            M_[1][1] *
                (temp2[columns - 2] - (double)(*src)(columns - 1, i, 0)) +
            M_[1][2] * (temp2[columns - 3] - (double)(*src)(columns - 1, i, 0));

        double temp2Wp1 =
            (double)(*src)(columns - 1, i, 0) +
            M_[2][0] *
                (temp2[columns - 1] - (double)(*src)(columns - 1, i, 0)) +
            M_[2][1] *
                (temp2[columns - 2] - (double)(*src)(columns - 1, i, 0)) +
            M_[2][2] * (temp2[columns - 3] - (double)(*src)(columns - 1, i, 0));

        temp2[columns - 1] = temp2Wm1;

        temp2[columns - 2] = B_ * temp2[columns - 2] +
                             b1_ * temp2[columns - 1] + b2_ * temp2W +
                             b3_ * temp2Wp1;

        temp2[columns - 3] = B_ * temp2[columns - 3] +
                             b1_ * temp2[columns - 2] +
                             b2_ * temp2[columns - 1] + b3_ * temp2W;

        for (int j = columns - 4; j != -1; j--)
            temp2[j] = B_ * temp2[j] + b1_ * temp2[j + 1] + b2_ * temp2[j + 2] +
                       b3_ * temp2[j + 3];

        for (int j = 0; j < columns; j++) (*src)(j, i, 0) = (double)temp2[j];
    }
}

void GaussFilter::update() {
    c_begin = clock();
    b_begin = boost::chrono::system_clock::now();
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    // memcpy(inputImage_->_data, this->source_ports[0].getData()->_data,
    //        inputImage_->size() * sizeof(double));
    inputImage_->assign(*(this->source_ports[0].getData()));

    // Do not invert the two computation steps
    gaussVertical(inputImage_);
    gaussHorizontal(inputImage_);

    *outputImage_ = *inputImage_;
    c_end = clock();
    b_end = boost::chrono::system_clock::now();
    this->elapsed_time += double(c_end - c_begin) / CLOCKS_PER_SEC;
    this->elapsed_wall_time +=
        ((boost::chrono::duration<double>)(b_end - b_begin)).count();
}

const CImg<double> *GaussFilter::getOutput() const { return outputImage_; }
