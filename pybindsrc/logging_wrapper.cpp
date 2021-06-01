#include "logging/Logging.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace py = pybind11;

namespace dunedaq {
namespace logging {
namespace python {

PYBIND11_MODULE(_daq_logging_py, top_module) {

    top_module.doc() = "Python module wrapper for the logging package"; // optional module docstring

    py::class_<logging::Logging>(top_module, "Logging")
        .def("setup", &logging::Logging::setup);
}

} // namespace python
} // namespace logging
} // namespace dunedaq