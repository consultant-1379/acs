/**
 * Main page source
 *
 * @file mainpage-spi.h
 *
 * @version 1
 */

/**
 * @mainpage COM SPI Documentation
 *
 * The COM Service Provider Interface (SPI) defines the contract between the COM and a COM Support Agent (SA).
 * The COM SPI is defined in C and provides interfaces in terms of function pointers, structs, and enums.
 * Some interfaces are implemented by the COM, some by the COM SA, and some by both parties.<p>
 *
 * The COM SPI is divided in three main sections:<br><ol>
 * <li>COM Management (MGMT) SPI, which provides the basic framework for components inspired by the OSGI standard.</li>
 * <li>COM Middleware (MW) SPI, which provides the COM with high-availability, log, and trace functionality.</li>
 * <li>COM OAM SPI, which provides managed object, transaction, and model repository interfaces.</li>
 * </ol>
 *
 * A prerequisite to use the information in this container is that the
 * COM SPI Programmer's Guide (3/19817-APR 901 0443/1) has been read.
 *
 * @n @b Document @b Structure @n
 * COM SPI Documentation is generated using Doxygen. The main structure of contents, based on the default layout of Doxygen,
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
 * All SPI header file names must include the version,
 * <b> &lt;name&gt;_n </b>  where @b n is a positive integer.
 * The same applies for the three main container header
 * files; their version must be stepped as soon as one of
 * the included files is changed.
 *
 */
