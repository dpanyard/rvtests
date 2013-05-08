#ifndef _LINEARREGRESSION_H_
#define _LINEARREGRESSION_H_

#include "MathMatrix.h"
#include "MathCholesky.h"
#include <cmath>
#include "MathStats.h"
#include "MathSVD.h"

//use Wald statistics
class LinearRegression{
  public:
    LinearRegression() {};
    ~LinearRegression() {};

    bool FitLinearModel(Matrix & X, Matrix & y); // return false if not converging
    bool FitLinearModel(Matrix & X, Vector & y); // return false if not converging
    /// double GetDeviance(Matrix & X, Vector & y);
	Vector & GetAsyPvalue();
	Vector & GetCovEst() {return this->B;} ; // (X'X)^{-1} X'Y
	Matrix & GetCovB() {return this->covB;};
    Vector & GetPredicted() { return this->predict;};
    Vector & GetResiduals() { return this->residuals;};
    double GetSigma2() {return this->sigma2;};
    Vector B;       // coefficient vector
    Matrix covB;    // coefficient covariance matrix
  private:
	Vector pValue;   //
    Vector residuals;  // Y - X' \hat(beta)
    Vector predict;  // Y - X' \hat(beta)rvtest.1110.tgzrvtest.1110.tgzrvtest.1110.tgz
    Matrix XtXinv;   // (X'X)^ {-1}
    Cholesky chol;
    double sigma2; // \hat{\sigma^2} MLE
};

#endif /* _LINEARREGRESSION_H_ */