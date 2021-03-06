

///@file optimize.cpp
///@brief functions for optimization
///@version 1.0

#include <iostream>
#include <fstream>
#include <assert.h>
#include <math.h>
#include <map>
#include <armadillo>

#include <boost/python.hpp>
#include <boost/numpy.hpp>

#include <smartplus/Libraries/Phase/phase_characteristics.hpp>
#include <smartplus/Libraries/Phase/read.hpp>
#include <smartplus/Libraries/Phase/write.hpp>
#include <smartplus/Libraries/Material/ODF.hpp>
#include <smartplus/Libraries/Material/ODF2Nphases.hpp>
#include <smartplus/Libraries/Material/read.hpp>

#include <simmit/arma2numpy/numpy_arma.hpp>
#include <simmit/smartplus_wrappers/Libraries/Material/ODF.hpp>

namespace bp = boost::python;
namespace bn = boost::numpy;
using namespace std;
using namespace arma;
using namespace arma2numpy;

namespace smartpy{
    
bn::ndarray get_densities(const bn::ndarray &x_py, const bp::str &path_data_py, const bp::str &peak_file_py, const bool &radian) {
    
    string path_data = bp::extract<std::string>(path_data_py);
    string peak_file = bp::extract<std::string>(peak_file_py);
    
    //transform x in a vec
    vec x = array2vec(x_py);
    //Get the densities
    vec y = smart::get_densities(x, path_data, peak_file, radian);
    //Get the densities
    return vec2array(y);
}
    
void ODF_discretization(const int &nphases_rve, const int &num_phase_disc, const double &angle_min, const double &angle_max, const bp::str &umat_name_py, const bn::ndarray &props_py, const bp::str &path_data_py, const bp::str &peak_file_py, const bp::str &rve_init_file_py, const bp::str &rve_disc_file_py, const int &angle_mat) {

    string umat_name = bp::extract<std::string>(umat_name_py);
    string path_data = bp::extract<std::string>(path_data_py);
    string peak_file = bp::extract<std::string>(peak_file_py);
    string rve_init_file = bp::extract<std::string>(rve_init_file_py);
    string rve_disc_file = bp::extract<std::string>(rve_disc_file_py);
    
    vec props = array2vec(props_py);
        
    double psi_rve = 0.;
    double theta_rve = 0.;
    double phi_rve = 0.;
    
    smart::phase_characteristics rve_init;
    rve_init.sptr_matprops->update(0, umat_name, 1, psi_rve, theta_rve, phi_rve, props.n_elem, props);
    
    string inputfile;
    string outputfile;
    
    std::map<string, int> list_umat;
    list_umat = {{"MIHEN",100},{"MIMTN",101},{"MISCN",102},{"MIPCW",103},{"MIPLN",104}};
    
    //Here we read everything about the initial rve
    switch (list_umat[rve_init.sptr_matprops->umat_name]) {
            
        case 100: case 101: case 102: case 103: {
            rve_init.construct(2,1); //The rve is supposed to be mechanical only here
            smart::read_ellipsoid(rve_init, path_data, rve_init_file);
            break;
        }
        case 104: {
            rve_init.construct(1,1); //The rve is supposed to be mechanical only here
            smart::read_layer(rve_init, path_data, rve_init_file);
            break;
        }
    }
    
    smart::ODF odf_rve(0, false, angle_min, angle_max);
    smart::read_peak(odf_rve, path_data, peak_file);
    
    smart::phase_characteristics rve = discretize_ODF(rve_init, odf_rve, num_phase_disc, nphases_rve, angle_mat);
    
    if(rve.shape_type == 0) {
        smart::write_phase(rve, path_data, rve_disc_file);
    }
    if(rve.shape_type == 1) {
        smart::write_layer(rve, path_data, rve_disc_file);
    }
    else if(rve.shape_type == 2) {
        smart::write_ellipsoid(rve, path_data, rve_disc_file);
    }
    else if(rve.shape_type == 3) {
        smart::write_cylinder(rve, path_data, rve_disc_file);
    }
    
    return;
}
    
} //namespace smart_py