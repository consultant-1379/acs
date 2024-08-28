/**
 * Main page source
 *
 * @file mainpage-api.h
 *
 * @version 1
 */

/**
 * @mainpage COM API Documentation
 *
 * The COM Application Programming Interface (API) defines the interface for COM adaptation components. An adaptation component is loaded by COM and provides means for COM customization. The COM API is defined in C and provides interfaces in terms of function pointers, structs, and enums.
 *
 * The COM API is divided in four main sections:
 *
 * COM CLI API, which provides the CLI command module interface.
 * COM Management (MGMT) SPI, which provides the basic framework for components inspired by the OSGI standard.
 * COM Middleware (MW) SPI, which provides the COM with high-availability, log, and trace functionality.
 * COM OAM SPI, which provides managed object, transaction, and model repository interfaces.
 * A prerequisite to use the information in this container is that the COM SPI Programmer's Guide (3/19817-APR 901 0443/1) has been read.
 *
 * @n @b Document @b Structure @n
 * COM API Documentation is generated using Doxygen. The main structure of contents, based on the default layout of Doxygen,
 * is as follows: @n
 * @li @b Structures - Data structures with brief descriptions. It includes all structures in header files
 * ordered in alphabetical order.
 * @li @b Files - Header files with brief descriptions. The files are divided into three sections:
 * Mgmt, MW, and OAM.
 * @li @b Functions - Structures and union fields with links to the structures and unions they belong to.
 * @li @b Globals - Functions, variables, defines, enums, and typedefs with links to the files they belong to.
 *
 *
 * @n @b Versions @n
 * All API header files names must include the version,
 * <b> &lt;name&gt;_n </b>  where @b n is a positive integer.
 * The same applies for the three main container header
 * files; their version must be stepped as soon as one of
 * the included files is changed.
 *
 */
