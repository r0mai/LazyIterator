
#include <boost/test/unit_test.hpp>
#include "ph.hpp"
#include "algorithm.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

BOOST_AUTO_TEST_SUITE(mainTests)


BOOST_AUTO_TEST_CASE(until_with_value_predicate_should_compare_equal_to_iterator_pointing_to_that_value) {
	std::vector<int> v = { 1, 2, 3 };

	auto iterator = ph::until([](const int& i) { return i == 2; });

	BOOST_CHECK(iterator == ++(v.begin()));
}

BOOST_AUTO_TEST_CASE(until_with_value_predicate_should_not_compare_equal_to_iterator_not_pointing_to_that_value) {
	std::vector<int> v = { 1, 2, 3 };

	auto iterator = ph::until([](const int& i) { return i == 5; });

	BOOST_CHECK(iterator != ++(v.begin()));
}

BOOST_AUTO_TEST_CASE(When_untils_are_composed_find_should_stop_when_any_of_them_compare_equal) {
	std::vector<int> v = { 1, 2, 3, 4 };

	auto iterator1 = ph::until([](const int& i) { return i == 3; });
	auto iterator2 = ph::until([](const int& i) { return i == 4; });

	auto pos = ph::find(v.begin(), iterator1 || iterator2, 5);

	BOOST_CHECK_EQUAL(*pos, 3);

}

BOOST_AUTO_TEST_CASE(Using_find_until_composition_should_be_commutative) {
	std::vector<int> v = { 1, 2, 3, 4 };

	auto iterator1 = ph::until([](const int& i) { return i == 3; });
	auto iterator2 = ph::until([](const int& i) { return i == 4; });

	auto pos1 = ph::find(v.begin(), iterator1 || iterator2, 5);
	auto pos2 = ph::find(v.begin(), iterator2 || iterator1, 5);

	BOOST_CHECK_EQUAL(std::addressof(*pos1), std::addressof(*pos2));

}

BOOST_AUTO_TEST_CASE(Using_find_until_should_work_if_until_predicates_are_runtime) {
	std::vector<int> v = { 1, 2, 3, 4 };

	const volatile int delimiter = 3;
	auto iterator = ph::until([=](const int& i) { return i == delimiter; });

	auto posPh = ph::find(v.begin(), iterator, 4);
	auto posStl = std::find(v.begin(), v.begin()+2, 4);

	BOOST_CHECK_EQUAL(*posPh, *posStl);
}

BOOST_AUTO_TEST_CASE(Using_find_with_until_vals_should_work_as_if_many_untils_were_composed) {
	std::vector<int> v = { 1, 2, 3, 4, 5 };

	auto iterator1 = ph::until([](const int& i) { return i == 3; });
	auto iterator2 = ph::until([](const int& i) { return i == 4; });
	auto iterator3 = ph::until([](const int& i) { return i == 5; });

	auto posComposite = ph::find(v.begin(), iterator1 || iterator2 || iterator3 || v.end(), 6);
	auto posUntilVal = ph::find(v.begin(), ph::untilValue(3, 4, 5) || v.end(), 6);

	BOOST_CHECK(posComposite == posUntilVal);
}

BOOST_AUTO_TEST_CASE(Using_max_element_should_find_largest_element_before_delimiter) {
	std::vector<int> v = { 1, 2, 3 };

	auto endIterator = ph::until([](const int& i) { return i==3; });

	auto maxPosPh = ph::max_element(v.begin(), endIterator);

	auto maxPosStl = std::max_element(v.begin(), v.begin()+2);

	BOOST_CHECK(maxPosPh == maxPosStl);
}

BOOST_AUTO_TEST_CASE(Using_min_element_should_find_smallest_element_before_delimiter) {
	std::vector<int> v = { 1, 2, 0 };

	auto endIterator = ph::until([](const int& i) { return i==0; });

	auto minPosPh = ph::min_element(v.begin(), endIterator);

	auto minPosStl = std::min_element(v.begin(), v.begin()+2);

	BOOST_CHECK(minPosPh == minPosStl);
}

BOOST_AUTO_TEST_CASE(Using_max_element_with_comp_should_find_largest_element_before_delimiter) {
	std::vector<int> v = { 1, 2, 3 };

	auto endIterator = ph::until([](const int& i) { return i==3; });

	auto maxPosPh = ph::max_element(v.begin(), endIterator, std::less<int>());

	auto maxPosStl = std::max_element(v.begin(), v.begin()+2, std::less<int>());

	BOOST_CHECK(maxPosPh == maxPosStl);
}

BOOST_AUTO_TEST_CASE(Using_min_element_with_comp_should_find_smallest_element_before_delimiter) {
	std::vector<int> v = { 1, 2, 0 };

	auto endIterator = ph::until([](const int& i) { return i==0; });

	auto minPosPh = ph::min_element(v.begin(), endIterator, std::less<int>());

	auto minPosStl = std::min_element(v.begin(), v.begin()+2, std::less<int>());

	BOOST_CHECK(minPosPh == minPosStl);
}

BOOST_AUTO_TEST_CASE(Until_for_each_should_visit_all_elements_before_delimiter) {
	std::vector<int> v = { 1, 2, 3, 4, 5 };

	auto endIterator = ph::until([](const int& i) { return i==5; });

	std::vector<int> visitedInts;

	ph::for_each(v.begin(), endIterator, [&visitedInts](const int& i) {
			visitedInts.push_back(i);
	});

	BOOST_REQUIRE_EQUAL(visitedInts.size(), 4);
	BOOST_CHECK_EQUAL(visitedInts[0], 1);
	BOOST_CHECK_EQUAL(visitedInts[1], 2);
	BOOST_CHECK_EQUAL(visitedInts[2], 3);
	BOOST_CHECK_EQUAL(visitedInts[3], 4);
}

BOOST_AUTO_TEST_CASE(Until_count_should_count_matching_elements_before_delimiter) {
	std::vector<int> v = { 1, 1, 3, 4, 1, 1 };

	auto endIterator = ph::until([](const int& i) { return i==4; });


	auto count = ph::count(v.begin(), endIterator, 1);
	BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_CASE(Until_count_should_not_run_over_the_end_if_the_delimiter_is_not_found) {
	std::vector<int> v = { 1, 1, 3, 4, 1, 1 };

	auto endIterator = ph::until([](const int& i) { return i==8; });


	auto count = ph::count(v.begin(), endIterator || v.end(), 1);
	BOOST_CHECK_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE(Until_count_should_not_run_over_an_arbitrary_real_iterator_if_the_delimiter_is_not_found) {
	std::vector<int> v = { 1, 1, 3, 4, 1, 1 };

	auto endIterator = ph::until([](const int& i) { return i==8; });


	auto count = ph::count(v.begin(), endIterator || (v.begin() + 4), 1);
	BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_CASE(Until_find_should_not_overstep_the_end_iterator_if_it_is_given_and_neither_value_nor_delimiter_is_found) {
	std::vector<int> v = { 1, 1, 1, 1, 1 };

	auto endIterator = ph::until([](const int&){ return false; });

	auto posPh = ph::find(v.begin(), endIterator || v.end(), 2);
	auto posStl = std::find(v.begin(), v.end(), 2);

	BOOST_CHECK(posPh == posStl);

}

BOOST_AUTO_TEST_CASE(Until_count_should_not_overstep_the_end_iterator_if_it_is_given_and_neither_value_nor_delimiter_is_found) {
	std::vector<int> v = { 1, 1, 1, 1, 1 };

	auto endIterator = ph::until([](const int&){ return false; });

	auto countPh = ph::count(v.begin(), endIterator || v.end(), 2);
	auto countStl = std::count(v.begin(), v.end(), 2);

	BOOST_CHECK_EQUAL(countPh, countStl);

}

BOOST_AUTO_TEST_CASE(equal_works_with_normal_iterators_positive_test) {
	std::vector<int> v1 = { 1, 2, 3, 4, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	BOOST_CHECK(ph::equal(v1.begin(), v1.end(), v2.begin()));
}

BOOST_AUTO_TEST_CASE(equal_works_with_normal_iterators_negative_test) {
	std::vector<int> v1 = { 1, 2, 3, 5, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	BOOST_CHECK(!ph::equal(v1.begin(), v1.end(), v2.begin()));
}

BOOST_AUTO_TEST_CASE(equal_works_with_until_iterators_positive_test) {
	std::vector<int> v1 = { 1, 2, 3, 10, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	auto endIterator = ph::until([](const int& i){ return i == 10; });

	BOOST_CHECK(ph::equal(v1.begin(), endIterator, v2.begin()));
}

BOOST_AUTO_TEST_CASE(equal_works_with_until_iterators_negative_test) {
	std::vector<int> v1 = { 1, 2, 4, 10, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	auto endIterator = ph::until([](const int& i){ return i == 10; });

	BOOST_CHECK(!ph::equal(v1.begin(), endIterator, v2.begin()));
}

BOOST_AUTO_TEST_CASE(equal_predicate_works_with_normal_iterators_positive_test) {
	std::vector<int> v1 = { 1, 2, 3, 4, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	BOOST_CHECK(ph::equal(v1.begin(), v1.end(), v2.begin(), std::equal_to<int>()));
}

BOOST_AUTO_TEST_CASE(equal_predicate_works_with_normal_iterators_negative_test) {
	std::vector<int> v1 = { 1, 2, 3, 5, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	BOOST_CHECK(!ph::equal(v1.begin(), v1.end(), v2.begin(), std::equal_to<int>()));
}

BOOST_AUTO_TEST_CASE(equal_predicate_works_with_until_iterators_positive_test) {
	std::vector<int> v1 = { 1, 2, 3, 10, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	auto endIterator = ph::until([](const int& i){ return i == 10; });

	BOOST_CHECK(ph::equal(v1.begin(), endIterator, v2.begin(), std::equal_to<int>()));
}

BOOST_AUTO_TEST_CASE(equal_predicate_works_with_until_iterators_negative_test) {
	std::vector<int> v1 = { 1, 2, 4, 10, 5 };
	std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };

	auto endIterator = ph::until([](const int& i){ return i == 10; });

	BOOST_CHECK(!ph::equal(v1.begin(), endIterator, v2.begin(), std::equal_to<int>()));
}

BOOST_AUTO_TEST_CASE(find_and_operator_works_if_it_gets_true) {
	std::vector<int> v1 = { 1, 2, 4, 6, 10, 5 };

	auto endIterator = ph::until([](const int& i){ return i % 3 == 0; }) &&
		ph::until([](const int& i){ return i % 2 == 0; });

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 6);
	BOOST_CHECK_EQUAL(it - v1.begin(), 3);
}

BOOST_AUTO_TEST_CASE(find_and_operator_works_if_it_doesnt_get_true) {
	std::vector<int> v1 = { 1, 2, 4, 7, 10, 5 };

	auto endIterator = ph::until([](const int& i){ return i % 3 == 0; }) &&
		ph::until([](const int& i){ return i % 2 == 0; });

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 5);
	BOOST_CHECK_EQUAL(it - v1.begin(), 5);
}

BOOST_AUTO_TEST_CASE(find_and_operator_works_with_or_real_end_if_it_gets_true) {
	std::vector<int> v1 = { 1, 2, 4, 6, 10, 5 };

	auto endIterator = (ph::until([](const int& i){ return i % 3 == 0; }) &&
		ph::until([](const int& i){ return i % 2 == 0; })) || v1.end();

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 6);
	BOOST_CHECK_EQUAL(it - v1.begin(), 3);
}

BOOST_AUTO_TEST_CASE(find_and_operator_works_with_or_real_end_if_it_doesnt_get_true) {
	std::vector<int> v1 = { 1, 2, 4, 7, 10, 5 };

	auto endIterator = (ph::until([](const int& i){ return i % 3 == 0; }) &&
		ph::until([](const int& i){ return i % 2 == 0; })) || v1.end();

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 5);
	BOOST_CHECK_EQUAL(it - v1.begin(), 5);
}

BOOST_AUTO_TEST_CASE(find_and_operator_works_with_or_real_end_if_end_is_reached) {
	std::vector<int> v1 = { 1, 2, 4, 7, 10, 7 };

	auto endIterator = (ph::until([](const int& i){ return i % 3 == 0; }) &&
		ph::until([](const int& i){ return i % 2 == 0; })) || v1.end();

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK(it == v1.end());
}

BOOST_AUTO_TEST_CASE(find_not_operator_works_when_its_reached) {
	std::vector<int> v1 = { 6, 12, 0, 4, 3, 5 };

	auto endIterator = !ph::until([](const int& i){ return i % 3 == 0; });

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 4);
	BOOST_CHECK_EQUAL(it - v1.begin(), 3);
}

BOOST_AUTO_TEST_CASE(find_not_operator_works_when_it_doesnt_get_reached) {
	std::vector<int> v1 = { 6, 12, 0, -3, 3, 5 };

	auto endIterator = !ph::until([](const int& i){ return i % 3 == 0; });

	auto it = ph::find(v1.begin(), endIterator, 5);

	BOOST_CHECK_EQUAL(*it, 5);
	BOOST_CHECK_EQUAL(it - v1.begin(), 5);
}

BOOST_AUTO_TEST_SUITE_END()

