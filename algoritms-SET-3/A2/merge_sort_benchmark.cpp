#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#define INSERTION_THRESHOLD 15

class ArrayGenerator {
 public:
  explicit ArrayGenerator(int seed, int min, int max) : gen_(seed) {
    distr_ = std::uniform_int_distribution<>(min, max);
  }

  void FillRandom(const std::vector<int>::iterator &begin,
                  const std::vector<int>::iterator &end) const {
    for (auto curr = begin; curr < end; ++curr) {
      *curr = distr_(gen_);
    }
  }

  void FillSortedDesc(const std::vector<int>::iterator &begin,
                      const std::vector<int>::iterator &end) const {
    FillRandom(begin, end);
    std::sort(begin, end, std::greater<>());
  }

  void FillAlmostSorted(const std::vector<int>::iterator &begin,
                        const std::vector<int>::iterator &end) const {
    FillRandom(begin, end);
    std::sort(begin, end);
    int len = static_cast<int>(std::distance(begin, end));
    int shuffle_cnt = len / 10;
    std::uniform_int_distribution<> index_distr(0, len - 1);
    for (size_t i = 0; i < shuffle_cnt; i++) {
      std::swap(*(begin + index_distr(gen_)), *(begin + index_distr(gen_)));
    }
  }

 private:
  mutable std::mt19937 gen_;
  mutable std::uniform_int_distribution<> distr_;
};

class SortTester {
 public:
  explicit SortTester(
      const std::function<void(const std::vector<int>::iterator &,
                               const std::vector<int>::iterator &)> &filler)
      : filler_(filler) {}

  void Benchmark(std::ostream &out,
                 const std::function<void(std::vector<int> &)> &sort) const {
    std::vector<int> data(10000, 0);
    filler_(data.begin(), data.end());

    for (int n = 500; n < 10000; n += 100) {
      std::vector<int> a(n, 0);
      std::copy(data.begin(), data.begin() + n, a.begin());

      auto start = std::chrono::high_resolution_clock::now();
      sort(a);
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      long long msec =
          std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
              .count();

      out << "N = " << n << ";T = " << msec << "\n";
    }
  }

 private:
  std::function<void(const std::vector<int>::iterator &,
                     const std::vector<int>::iterator &)>
      filler_;
};

void insertion_sort(std::vector<int> &a) {
  for (int i = 1; i < a.size(); ++i) {
    int j = i - 1;
    const int key = a[i];
    while (j >= 0 && key < a[j]) {
      a[j + 1] = a[j];
      j--;
    }
    a[j + 1] = key;
  }
}

void merge(const std::vector<int> &left, const std::vector<int> &right,
           std::vector<int> &a) {
  int i = 0;
  int j = 0;
  while (i + j < a.size()) {
    if (j >= right.size() || i < left.size() && !(right[j] < left[i])) {
      a[i + j] = left[i];
      i++;
    } else {
      a[i + j] = right[j];
      j++;
    }
  }
}

void merge_sort(std::vector<int> &a) {
  if (a.size() < 2) {
    return;
  }

  std::vector<int> left(a.size() / 2);
  std::vector<int> right((a.size() + 1) / 2);
  std::copy(a.begin(), a.begin() + left.size(), left.begin());
  std::copy(a.begin() + left.size(), a.end(), right.begin());

  merge_sort(left);
  merge_sort(right);
  merge(left, right, a);
}

void merge_insertion_sort(std::vector<int> &a) {  // NOLINT
  if (a.size() < INSERTION_THRESHOLD) {
    insertion_sort(a);
    return;
  }

  std::vector<int> left(a.size() / 2);
  std::vector<int> right((a.size() + 1) / 2);
  std::copy(a.begin(), a.begin() + left.size(), left.begin());
  std::copy(a.begin() + left.size(), a.end(), right.begin());

  merge_insertion_sort(left);
  merge_insertion_sort(right);
  merge(left, right, a);
}

int main() {
  ArrayGenerator gen(169, 0, 6000);  // Fixed seed.
  auto filler = [&](auto begin, auto end) -> void {
    gen.FillAlmostSorted(begin, end);
  };
  SortTester tester(filler);
  tester.Benchmark(std::cout, merge_insertion_sort);
}