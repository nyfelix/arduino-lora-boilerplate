#pragma once

class logistic_regression
{
public:
    explicit logistic_regression(
        const double intercept,
        const double *coef,
        const unsigned short nof_coef,
        const double euler,
        double (*pow)(double, double)) : intercept{intercept},
                                         coef_{coef},
                                         nof_coef{nof_coef},
                                         euler_{euler},
                                         pow_{pow}
    {
    }
    template <typename T>
    double predict(T *matrix)
    {
        double sum{multiply_and_sum(matrix) + intercept};
        return 1 / (1 + pow_(euler_, -sum));
    }

    template <typename T>
    double multiply_and_sum(T *matrix)
    {
        double sum{};
        for (auto index = 0; index < nof_coef; index++)
        {
            sum += matrix[index] * coef_[index];
        }
        return sum;
    }

private:
    const double intercept{};
    const double *coef_;
    const unsigned short nof_coef{};
    const double euler_{};
    double (*pow_)(double, double);
};