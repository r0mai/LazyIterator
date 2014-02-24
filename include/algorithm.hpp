#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

// The below algorithms are ports from
// http://en.cppreference.com/w/cpp/header/algorithm
//
// The main changes to them are that instead of requiring a single iterator
// type for both the begin and the end iterator, the simply require that the
// begin be a forward iterator, and that begin is comparable to end.


namespace ph {

template<typename Begin, typename End, typename ValueType>
Begin find(Begin begin, End end, const ValueType& value) {
	for(; end != begin; ++begin) {
		if(*begin == value) {
			return begin;
		}
	}
	return begin;
}

template<typename Begin, typename End>
Begin max_element(Begin begin, End end) {
	Begin answerIterator = begin++;

	for(; end != begin; ++begin) {
		if(*begin > *answerIterator)
			answerIterator = begin;
	}

	return answerIterator;
}

template<typename Begin, typename End, class UnaryFunction>
UnaryFunction for_each(Begin begin, End end, UnaryFunction f) {
	for (; end != begin; ++begin) {
		f(*begin);
	}
	return f;
}

template<class Begin, class End, class T>
std::size_t count(Begin begin, End end, const T& value) {
	std::size_t ret = 0;
    for (; end != begin; ++begin) {
        if (*begin == value) {
            ret++;
        }
    }
    return ret;
}

} // namespace ph

#endif /* ALGORITHM_HPP_ */
