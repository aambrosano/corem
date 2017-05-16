#include <COREM/core/module/spaceVariantGaussFilter.h>
#include <cassert>

SpaceVariantGaussFilter::SpaceVariantGaussFilter(std::string id,
                                                 retina_config_t *conf,
                                                 double sigma, double K,
                                                 double R0)
    : Module(id, conf) {
    K_ = K;
    R0_ = R0;
    sigma_ = sigma;

    // transform sigma to pixels
    sigma_ *= conf->pixelsPerDegree;

    // TODO: why 0.1?
    inputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);
    outputImage_ = new CImg<double>(columns, rows, 1, 1, 0.1);

    // reserve space for all possible threads
    buffer_ = new double[(columns + rows) * omp_get_max_threads()];

    double new_sigma = 0.0;
    double r;

    // TODO: why 0.1?
    // initialize matrices
    q_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    b0_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    b1_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    b2_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    b3_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    B_m_ = CImg<double>(columns, rows, 1, 1, 0.1);
    M_m_ = CImg<double>(columns, rows, 9, 1, 0.1);

    for (unsigned int i = 0; i < columns; i++) {
        for (unsigned int j = 0; j < rows; j++) {
            // update sigma value
            r = sqrt((double(i) - floor(columns / 2)) *
                         (double(i) - floor(columns / 2)) +
                     (double(j) - floor(rows / 2)) *
                         (double(j) - floor(rows / 2)));
            new_sigma = sigma_ / density(r / conf->pixelsPerDegree);

            // coefficient calculation
            q_m_(i, j, 0) = 0.98711 * new_sigma - 0.96330;

            if (new_sigma < 2.5)
                q_m_(i, j, 0) =
                    3.97156 - 4.14554 * sqrt(1.0 - 0.26891 * new_sigma);

            b0_m_(i, j, 0) =
                1.57825 + 2.44413 * q_m_(i, j, 0) +
                1.4281 * q_m_(i, j, 0) * q_m_(i, j, 0) +
                0.422205 * q_m_(i, j, 0) * q_m_(i, j, 0) * q_m_(i, j, 0);
            b1_m_(i, j, 0) =
                2.44413 * q_m_(i, j, 0) +
                2.85619 * q_m_(i, j, 0) * q_m_(i, j, 0) +
                1.26661 * q_m_(i, j, 0) * q_m_(i, j, 0) * q_m_(i, j, 0);
            b2_m_(i, j, 0) =
                -1.4281 * q_m_(i, j, 0) * q_m_(i, j, 0) -
                1.26661 * q_m_(i, j, 0) * q_m_(i, j, 0) * q_m_(i, j, 0);
            b3_m_(i, j, 0) =
                0.422205 * q_m_(i, j, 0) * q_m_(i, j, 0) * q_m_(i, j, 0);
            B_m_(i, j, 0) = 1.0 -
                            (b1_m_(i, j, 0) + b2_m_(i, j, 0) + b3_m_(i, j, 0)) /
                                b0_m_(i, j, 0);

            b1_m_(i, j, 0) /= b0_m_(i, j, 0);
            b2_m_(i, j, 0) /= b0_m_(i, j, 0);
            b3_m_(i, j, 0) /= b0_m_(i, j, 0);

            // From: Bill Triggs, Michael Sdika: Boundary Conditions for
            // Young-van
            // Vliet Recursive Filtering
            M_m_(i, j, 0) = -b3_m_(i, j, 0) * b1_m_(i, j, 0) + 1.0 -
                            b3_m_(i, j, 0) * b3_m_(i, j, 0) - b2_m_(i, j, 0);
            M_m_(i, j, 1) = (b3_m_(i, j, 0) + b1_m_(i, j, 0)) *
                            (b2_m_(i, j, 0) + b3_m_(i, j, 0) * b1_m_(i, j, 0));
            M_m_(i, j, 2) = b3_m_(i, j, 0) *
                            (b1_m_(i, j, 0) + b3_m_(i, j, 0) * b2_m_(i, j, 0));
            M_m_(i, j, 3) = b1_m_(i, j, 0) + b3_m_(i, j, 0) * b2_m_(i, j, 0);
            M_m_(i, j, 4) = -(b2_m_(i, j, 0) - 1.0) *
                            (b2_m_(i, j, 0) + b3_m_(i, j, 0) * b1_m_(i, j, 0));
            M_m_(i, j, 5) =
                -(b3_m_(i, j, 0) * b1_m_(i, j, 0) +
                  b3_m_(i, j, 0) * b3_m_(i, j, 0) + b2_m_(i, j, 0) - 1.0) *
                b3_m_(i, j, 0);
            M_m_(i, j, 6) = b3_m_(i, j, 0) * b1_m_(i, j, 0) + b2_m_(i, j, 0) +
                            b1_m_(i, j, 0) * b1_m_(i, j, 0) -
                            b2_m_(i, j, 0) * b2_m_(i, j, 0);
            M_m_(i, j, 7) = b1_m_(i, j, 0) * b2_m_(i, j, 0) +
                            b3_m_(i, j, 0) * b2_m_(i, j, 0) * b2_m_(i, j, 0) -
                            b1_m_(i, j, 0) * b3_m_(i, j, 0) * b3_m_(i, j, 0) -
                            b3_m_(i, j, 0) * b3_m_(i, j, 0) * b3_m_(i, j, 0) -
                            b3_m_(i, j, 0) * b2_m_(i, j, 0) + b3_m_(i, j, 0);
            M_m_(i, j, 8) = b3_m_(i, j, 0) *
                            (b1_m_(i, j, 0) + b3_m_(i, j, 0) * b2_m_(i, j, 0));
            for (int z = 0; z < 9; z++)
                M_m_(i, j, z) /=
                    (1.0 + b1_m_(i, j, 0) - b2_m_(i, j, 0) + b3_m_(i, j, 0)) *
                    (1.0 + b2_m_(i, j, 0) +
                     (b1_m_(i, j, 0) - b3_m_(i, j, 0)) * b3_m_(i, j, 0));
        }
    }
}

SpaceVariantGaussFilter::~SpaceVariantGaussFilter() {
    delete inputImage_;
    delete outputImage_;
    delete buffer_;
}

void SpaceVariantGaussFilter::gaussHorizontal(CImg<double> &src) {
#pragma omp parallel for

    for (int i = 0; i < rows; i++) {
        double *temp2 = buffer_ + omp_get_thread_num() * (columns + rows);

        temp2[0] = B_m_(0, i, 0) * (double)src(0, i, 0) +
                   b1_m_(0, i, 0) * (double)src(0, i, 0) +
                   b2_m_(0, i, 0) * (double)src(0, i, 0) +
                   b3_m_(0, i, 0) * (double)src(0, i, 0);

        temp2[1] = B_m_(1, i, 0) * (double)src(1, i, 0) +
                   b1_m_(1, i, 0) * temp2[0] +
                   b2_m_(1, i, 0) * (double)src(0, i, 0) +
                   b3_m_(1, i, 0) * (double)src(0, i, 0);

        temp2[2] = B_m_(2, i, 0) * (double)src(2, i, 0) +
                   b1_m_(2, i, 0) * temp2[1] + b2_m_(2, i, 0) * temp2[0] +
                   b3_m_(2, i, 0) * (double)src(0, i, 0);

        for (unsigned int j = 3; j < columns; j++)
            temp2[j] = B_m_(j, i, 0) * (double)src(j, i, 0) +
                       b1_m_(j, i, 0) * temp2[j - 1] +
                       b2_m_(j, i, 0) * temp2[j - 2] +
                       b3_m_(j, i, 0) * temp2[j - 3];

        double temp2Wm1 =
            (double)src(columns - 1, i, 0) +
            M_m_(0, i, 0) *
                (temp2[columns - 1] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 1) *
                (temp2[columns - 2] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 2) *
                (temp2[columns - 3] - (double)src(columns - 1, i, 0));

        double temp2W =
            (double)src(columns - 1, i, 0) +
            M_m_(0, i, 3) *
                (temp2[columns - 1] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 4) *
                (temp2[columns - 2] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 5) *
                (temp2[columns - 3] - (double)src(columns - 1, i, 0));

        double temp2Wp1 =
            (double)src(columns - 1, i, 0) +
            M_m_(0, i, 6) *
                (temp2[columns - 1] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 7) *
                (temp2[columns - 2] - (double)src(columns - 1, i, 0)) +
            M_m_(0, i, 8) *
                (temp2[columns - 3] - (double)src(columns - 1, i, 0));

        temp2[columns - 1] = temp2Wm1;

        temp2[columns - 2] = B_m_(columns - 2, i, 0) * temp2[columns - 2] +
                             b1_m_(columns - 2, i, 0) * temp2[columns - 1] +
                             b2_m_(columns - 2, i, 0) * temp2W +
                             b3_m_(columns - 2, i, 0) * temp2Wp1;

        temp2[columns - 3] = B_m_(columns - 3, i, 0) * temp2[columns - 3] +
                             b1_m_(columns - 3, i, 0) * temp2[columns - 2] +
                             b2_m_(columns - 3, i, 0) * temp2[columns - 1] +
                             b3_m_(columns - 3, i, 0) * temp2W;

        for (unsigned int j = columns - 4; j != static_cast<unsigned int>(-1);
             j--)
            temp2[j] =
                B_m_(j, i, 0) * temp2[j] + b1_m_(j, i, 0) * temp2[j + 1] +
                b2_m_(j, i, 0) * temp2[j + 2] + b3_m_(j, i, 0) * temp2[j + 3];

        for (unsigned int j = 0; j < columns; j++)
            src(j, i, 0) = (double)temp2[j];
    }
}

void SpaceVariantGaussFilter::gaussVertical(CImg<double> &src) {
#pragma omp parallel for

    for (int i = 0; i < columns; i++) {
        double *temp2 = buffer_ + omp_get_thread_num() * (columns + rows);

        temp2[0] = B_m_(i, 0, 0) * (double)src(i, 0, 0) +
                   b1_m_(i, 0, 0) * (double)src(i, 0, 0) +
                   b2_m_(i, 0, 0) * (double)src(i, 0, 0) +
                   b3_m_(i, 0, 0) * (double)src(i, 0, 0);

        temp2[1] = B_m_(i, 1, 0) * (double)src(i, 1, 0) +
                   b1_m_(i, 1, 0) * temp2[0] +
                   b2_m_(i, 1, 0) * (double)src(i, 0, 0) +
                   b3_m_(i, 1, 0) * (double)src(i, 0, 0);

        temp2[2] = B_m_(i, 2, 0) * (double)src(i, 2, 0) +
                   b1_m_(i, 2, 0) * temp2[1] + b2_m_(i, 2, 0) * temp2[0] +
                   b3_m_(i, 2, 0) * (double)src(i, 0, 0);

        for (unsigned int j = 3; j < rows; j++)
            temp2[j] = B_m_(i, j, 0) * (double)src(i, j, 0) +
                       b1_m_(i, j, 0) * temp2[j - 1] +
                       b2_m_(i, j, 0) * temp2[j - 2] +
                       b3_m_(i, j, 0) * temp2[j - 3];

        double temp2Wm1 =
            (double)src(i, rows - 1, 0) +
            M_m_(i, 0, 0) * (temp2[rows - 1] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 1) * (temp2[rows - 2] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 2) * (temp2[rows - 3] - (double)src(i, rows - 1, 0));

        double temp2W =
            (double)src(i, rows - 1, 0) +
            M_m_(i, 0, 3) * (temp2[rows - 1] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 4) * (temp2[rows - 2] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 5) * (temp2[rows - 3] - (double)src(i, rows - 1, 0));

        double temp2Wp1 =
            (double)src(i, rows - 1, 0) +
            M_m_(i, 0, 6) * (temp2[rows - 1] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 7) * (temp2[rows - 2] - (double)src(i, rows - 1, 0)) +
            M_m_(i, 0, 8) * (temp2[rows - 3] - (double)src(i, rows - 1, 0));

        temp2[rows - 1] = temp2Wm1;

        temp2[rows - 2] = B_m_(i, rows - 2, 0) * temp2[rows - 2] +
                          b1_m_(i, rows - 2, 0) * temp2[rows - 1] +
                          b2_m_(i, rows - 2, 0) * temp2W +
                          b3_m_(i, rows - 2, 0) * temp2Wp1;

        temp2[rows - 3] = B_m_(i, rows - 3, 0) * temp2[rows - 3] +
                          b1_m_(i, rows - 3, 0) * temp2[rows - 2] +
                          b2_m_(i, rows - 3, 0) * temp2[rows - 1] +
                          b3_m_(i, rows - 3, 0) * temp2W;

        for (unsigned int j = rows - 4; j != static_cast<unsigned int>(-1); j--)
            temp2[j] =
                B_m_(i, j, 0) * temp2[j] + b1_m_(i, j, 0) * temp2[j + 1] +
                b2_m_(i, j, 0) * temp2[j + 2] + b3_m_(i, j, 0) * temp2[j + 3];

        for (unsigned int j = 0; j < rows; j++) src(i, j, 0) = (double)temp2[j];
    }
}

void SpaceVariantGaussFilter::update() {
    // The module is not connected
    if (this->source_ports.size() == 0) return;

    inputImage_->assign(this->source_ports[0].getData());

    gaussVertical(*inputImage_);
    gaussHorizontal(*inputImage_);

    *outputImage_ = *inputImage_;
}

CImg<double> *SpaceVariantGaussFilter::getOutput() { return outputImage_; }

double SpaceVariantGaussFilter::density(double r) {
    // cell density
    // r in degrees
    return (r > R0_) ? 1 / (1 + K_ * (r - R0_)) : 1;
}
