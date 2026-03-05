
#include <set>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdint.h>
#include <optional>
#include <type_traits>
#include <stdexcept>
#include <ostream>
#include <iostream>
#include <string>
#include <algorithm>

struct BufferUpdateRange {
	auto begin () {return mRanges.begin ();}
	auto end   () {return mRanges.end   ();}
	auto size  () {return mRanges.size  ();}

	void InsertRange (size_t begin, size_t end) {
		size_t _idx = 0;

		size_t _extendedRange = 0;
		bool _shouldConcatenate = false;
		for (auto & _range : mRanges) {
			if (begin >= _range.first && begin <= _range.first + _range.second) {
				if (begin + end <= _range.first + _range.second) {
					return;
				}
				_range.second = begin - _range.first + end;
				_extendedRange = ConcatenateWith (_idx, begin, end);
				_shouldConcatenate = true;
				break;
			}

			++_idx;
		}

		if (_extendedRange > 0) {
			size_t _lastIdx = _idx + _extendedRange;
			size_t _lastEnd = mRanges[_lastIdx].second + mRanges[_lastIdx].first;
			mRanges.erase (mRanges.begin () + _idx + 1, mRanges.begin () + _lastIdx + 1);
			mRanges[_idx].second = _lastEnd;
		}

		else if (!_shouldConcatenate) {
			mRanges.emplace_back (begin, end);
		}

		std::sort (mRanges.begin (), mRanges.end ());
	}

	void RemoveRange (size_t begin, size_t end) {
		size_t begin_idx = 0;
		size_t end_idx = 0;

		for (begin_idx = 0; begin_idx < mRanges.size (); ++begin_idx) {
			const auto & _range = mRanges[begin_idx];
			if (begin >= _range.first && begin <= _range.first + _range.second) {
				break;
			}
		}

		for (end_idx = begin_idx; end_idx < mRanges.size (); ++end_idx) {
			const auto & _range = mRanges[end_idx];
			if (begin + end >= _range.first && begin + end <= _range.first + _range.second) {
				break;
			}

			if (end_idx + 1 < mRanges.size ()) {
				if (begin + end > _range.first + _range.second && begin + end < mRanges[end_idx + 1].first) {
					++end_idx;
					break;
				}
			}
		}

		std::cout << begin_idx << "_" << end_idx << "\n";

		if (begin_idx == end_idx) {
			size_t nBegin = begin + end;
			size_t nEnd   = mRanges[begin_idx].second - nBegin;

			mRanges.emplace (mRanges.begin () + begin_idx + 1, nBegin, nEnd);
			mRanges[begin_idx].second = begin - mRanges[begin_idx].first;
			return;
		}

		if (mRanges[begin_idx].first != begin) {
			mRanges[begin_idx].second = begin - mRanges[begin_idx].first;
			++begin_idx;
		}

		if (mRanges[end_idx].first + mRanges[end_idx].second != begin + end) {
			//mRanges[end_idx].second = begin + end - mRanges[end_idx].first;
			mRanges[end_idx].first = begin + end;
			--end_idx;
		}

		if (begin_idx != end_idx + 1) {
			mRanges.erase (mRanges.begin () + begin_idx, mRanges.begin () + end_idx + 1);
		}

		std::sort (mRanges.begin (), mRanges.end ());
	}

	friend std::ostream& operator<< (std::ostream &out, const BufferUpdateRange & data) {
		out << "(\n";
		for (const auto & range : data.mRanges) {
			out << "\t[" << range.first << ", " << range.second << "]\n";
		}
		out << ")\n";

		return out;
	}


private:
	size_t ConcatenateWith (size_t idx, size_t begin, size_t end) const {
		size_t _result = 0;
		for (size_t i = idx + 1; i < mRanges.size (); ++i) {
			const auto & _currentRange = mRanges[i];

			if (_currentRange.first > begin + end) break;

			++_result;
		}
		
		return _result;
	}

	std::vector<std::pair<size_t, size_t>> mRanges;
};


int main () {
	
	BufferUpdateRange dirtyCommands;

	dirtyCommands.InsertRange (0, 10);
	std::cout << "Inserting (0, 10): " << dirtyCommands;

	dirtyCommands.InsertRange (20, 10);
	std::cout << "Inserting (20, 10): " << dirtyCommands;

	dirtyCommands.InsertRange (40, 10);
	std::cout << "Inserting (40, 10): " << dirtyCommands;

	dirtyCommands.InsertRange (60, 10);
	std::cout << "Inserting (60, 10): " << dirtyCommands;

	dirtyCommands.InsertRange (10, 30);
	std::cout  << "Inserting (10, 30): " << dirtyCommands;
	
	dirtyCommands.RemoveRange (10, 20);
	std::cout << "Removing (10, 20): " << dirtyCommands;

	dirtyCommands.RemoveRange (40, 25);
	std::cout << "Removing (40, 25): " << dirtyCommands;

	dirtyCommands.RemoveRange (10, 55);
	std::cout << "Removing (10, 55): " << dirtyCommands;

	dirtyCommands.InsertRange (25, 30);
	std::cout << "Inserting (25, 30): " << dirtyCommands;

	dirtyCommands.RemoveRange (10, 50);
	std::cout << "Removing (10, 50): " << dirtyCommands;

	return (0);
}