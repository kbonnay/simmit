
#include <armadillo>
#include <boost/python.hpp>
#include <boost/numpy.hpp>
#include <arma2numpy/numpy_arma.hpp>

#include <smartplus/Libraries/Solver/read.hpp>
#include <simmit/smartplus_wrappers/Libraries/Solver/read.hpp>

namespace bn = boost::numpy;
using namespace std;
using namespace arma;
using namespace arma2numpy;

namespace smartpy {

//This function reads material properties to prepare a simulation
void read_matprops(int &nprops, bn::ndarray &props, int &nstatev, double &rho, double &c_p) {
    vec v = array2vec(props);
    string umat_name;
    smart::read_matprops(umat_name, nprops, v, nstatev, rho, c_p);
    props = vec2array(v);
}
        
} //namepsace smartpy