/*
 * VCluster_unit_tests.hpp
 *
 *  Created on: May 9, 2015
 *      Author: Pietro incardona
 */

#ifndef VCLUSTER_UNIT_TESTS_HPP_
#define VCLUSTER_UNIT_TESTS_HPP_

#include "VCluster.hpp"
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include "timer.hpp"
#include <random>
#include "VCluster_unit_test_util.hpp"
#include "Point_test.hpp"

BOOST_AUTO_TEST_SUITE( VCluster_test )

/*! \brief calculate the x mob m
 *
 * \param x
 * \param m
 *
 */
int mod(int x, int m) {
    return (x%m + m)%m;
}

BOOST_AUTO_TEST_CASE( VCluster_use_reductions)
{
	Vcluster & vcl = *global_v_cluster;

	unsigned char uc = 1;
	char c = 1;
	short s = 1;
	unsigned short us = 1;
	int i = 1;
	unsigned int ui = 1;
	long int li = 1;
	unsigned long int uli = 1;
	float f = 1;
	double d = 1;

	unsigned char uc_max = vcl.getProcessUnitID();
	char c_max = vcl.getProcessUnitID();
	short s_max = vcl.getProcessUnitID();
	unsigned short us_max = vcl.getProcessUnitID();
	int i_max = vcl.getProcessUnitID();
	unsigned int ui_max = vcl.getProcessUnitID();
	long int li_max = vcl.getProcessUnitID();
	unsigned long int uli_max = vcl.getProcessUnitID();
	float f_max = vcl.getProcessUnitID();
	double d_max = vcl.getProcessUnitID();

	// Sum reductions
	if ( vcl.getProcessingUnits() < 128 )
		vcl.reduce(c);
	if ( vcl.getProcessingUnits() < 256 )
		vcl.reduce(uc);
	if ( vcl.getProcessingUnits() < 32768 )
		vcl.reduce(s);
	if ( vcl.getProcessingUnits() < 65536 )
		vcl.reduce(us);
	if ( vcl.getProcessingUnits() < 2147483648 )
		vcl.reduce(i);
	if ( vcl.getProcessingUnits() < 4294967296 )
		vcl.reduce(ui);
	vcl.reduce(li);
	vcl.reduce(uli);
	vcl.reduce(f);
	vcl.reduce(d);

	// Max reduction
	if ( vcl.getProcessingUnits() < 128 )
		vcl.max(c_max);
	if ( vcl.getProcessingUnits() < 256 )
		vcl.max(uc_max);
	if ( vcl.getProcessingUnits() < 32768 )
		vcl.max(s_max);
	if ( vcl.getProcessingUnits() < 65536 )
		vcl.max(us_max);
	if ( vcl.getProcessingUnits() < 2147483648 )
		vcl.max(i_max);
	if ( vcl.getProcessingUnits() < 4294967296 )
		vcl.max(ui_max);
	vcl.max(li_max);
	vcl.max(uli_max);
	vcl.max(f_max);
	vcl.max(d_max);
	vcl.execute();

	if ( vcl.getProcessingUnits() < 128 )
	{BOOST_REQUIRE_EQUAL(c_max,vcl.getProcessingUnits()-1);}
	if ( vcl.getProcessingUnits() < 256 )
	{BOOST_REQUIRE_EQUAL(uc_max,vcl.getProcessingUnits()-1);}
	if ( vcl.getProcessingUnits() < 32768 )
	{BOOST_REQUIRE_EQUAL(s_max,vcl.getProcessingUnits()-1);}
	if ( vcl.getProcessingUnits() < 65536 )
	{BOOST_REQUIRE_EQUAL(us_max,vcl.getProcessingUnits()-1);}
	if ( vcl.getProcessingUnits() < 2147483648 )
	{BOOST_REQUIRE_EQUAL(i_max,vcl.getProcessingUnits()-1);}
	if ( vcl.getProcessingUnits() < 4294967296 )
	{BOOST_REQUIRE_EQUAL(ui_max,vcl.getProcessingUnits()-1);}

	BOOST_REQUIRE_EQUAL(li_max,vcl.getProcessingUnits()-1);
	BOOST_REQUIRE_EQUAL(uli_max,vcl.getProcessingUnits()-1);
	BOOST_REQUIRE_EQUAL(f_max,vcl.getProcessingUnits()-1);
	BOOST_REQUIRE_EQUAL(d_max,vcl.getProcessingUnits()-1);
}

#define N_V_ELEMENTS 16
#define P_STRIDE 17

BOOST_AUTO_TEST_CASE(VCluster_send_recv)
{
	unsigned char uc = 1;
	char c = 1;
	short s = 1;
	unsigned short us = 1;
	int i = 1;
	unsigned int ui = 1;
	long int li = 1;
	unsigned long int uli = 1;
	float f = 1;
	double d = 1;

	Vcluster & vcl = *global_v_cluster;

	// check an all to all patter with the primitives

/*	for (size_t k = 0 ; k < vcl.getProcessingUnits() ; k++)
	{
		vcl.send(k,uc);
		vcl.send(k,c);
		vcl.send(k,s);
		vcl.send(k,us);
		vcl.send(k,i)
	}*/

	// check Long distance pattern with vectors

	typedef Point_test<float> p;

	Point_test<float> pt;
	openfpm::vector<Point_test<float>> v_send;

	pt.setx(vcl.getProcessUnitID());
	pt.sety(vcl.getProcessUnitID());
	pt.setz(vcl.getProcessUnitID());
	pt.sets(vcl.getProcessUnitID());

	pt.setv(0,vcl.getProcessUnitID());
	pt.setv(1,vcl.getProcessUnitID());
	pt.setv(2,vcl.getProcessUnitID());

	pt.sett(0,0,vcl.getProcessUnitID());
	pt.sett(0,1,vcl.getProcessUnitID());
	pt.sett(0,2,vcl.getProcessUnitID());
	pt.sett(1,0,vcl.getProcessUnitID());
	pt.sett(1,1,vcl.getProcessUnitID());
	pt.sett(1,2,vcl.getProcessUnitID());
	pt.sett(2,0,vcl.getProcessUnitID());
	pt.sett(2,1,vcl.getProcessUnitID());
	pt.sett(2,2,vcl.getProcessUnitID());


	// ADD n elements
	for (size_t i = 0 ; i < N_V_ELEMENTS ; i++)
		v_send.add(pt);

	// Send to 8 processors
	for (size_t i = 0 ; i < 8 ; i++)
		vcl.send( mod(vcl.getProcessUnitID() + i * P_STRIDE, vcl.getProcessingUnits()) ,i,v_send);

	openfpm::vector<openfpm::vector<Point_test<float>> > pt_buf;
	pt_buf.resize(8);

	// Recv from 8 processors
	for (size_t i = 0 ; i < 8 ; i++)
	{
		pt_buf.get(i).resize(N_V_ELEMENTS);
		vcl.recv( mod( (vcl.getProcessUnitID() - i * P_STRIDE), vcl.getProcessingUnits()) ,i,pt_buf.get(i));
	}

	vcl.execute();

	// Check the received buffers (carefull at negative modulo)
	for (size_t i = 0 ; i < 8 ; i++)
	{
		for (size_t j = 0 ; j < N_V_ELEMENTS ; j++)
		{
			Point_test<float> pt = pt_buf.get(i).get(j);

			size_t p_recv = mod( (vcl.getProcessUnitID() - i * P_STRIDE), vcl.getProcessingUnits());

			BOOST_REQUIRE_EQUAL(pt.template get<p::x>(),p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::y>(),p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::z>(),p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::s>(),p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::v>()[0],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::v>()[1],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::v>()[2],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[0][0],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[0][1],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[0][2],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[1][0],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[1][1],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[1][2],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[2][0],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[2][1],p_recv);
			BOOST_REQUIRE_EQUAL(pt.template get<p::t>()[2][2],p_recv);
		}
	}
}

BOOST_AUTO_TEST_CASE( VCluster_use_sendrecv)
{
	std::cout << "VCluster unit test start" << "\n";

	totp_check = true;
	test<PCX>();
	totp_check = false;
	test<NBX>();

	std::cout << "VCluster unit test stop" << "\n";
}

BOOST_AUTO_TEST_SUITE_END()


#endif /* VCLUSTER_UNIT_TESTS_HPP_ */
