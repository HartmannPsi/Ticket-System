#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu {

class exception {
protected:
  const std::string variant = "";
  std::string detail = "";

public:
  exception() {}
  exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
  virtual std::string what() { return variant + " " + detail; }
};

class index_out_of_bound : public exception {
  /* __________________________ */
};

class runtime_error : public exception {
  /* __________________________ */
};

class invalid_iterator : public exception {
  /* __________________________ */
};

class container_is_empty : public exception {
  /* __________________________ */
};
} // namespace sjtu

/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T> class vector {
public:
  /**
   * TODO
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  /**
   * you can see RandomAccessIterator at CppReference for help.
   */
  class const_iterator;
  class iterator {
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the
    // following typedef) to work properly. In particular, without the following
    // code,
    // @code{std::sort(iter, iter1);} would not compile.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type:
    // https://blog.csdn.net/u014299153/article/details/72419713 About
    // iterator_category: https://en.cppreference.com/w/cpp/iterator
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;
    friend class const_iterator;

  private:
    std::size_t ptr;
    vector<T> *father;

  public:
    iterator(const std::size_t &ptr_, vector<T> *father_)
        : ptr(ptr_), father(father_) {}
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const {
      return iterator(ptr + n, father);
      // TODO
    }
    iterator operator-(const int &n) const {
      return iterator(ptr - n, father);
      // TODO
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const iterator &rhs) const {

      if (father != rhs.father) {
        throw sjtu::invalid_iterator();
      }

      return ptr - rhs.ptr;
      // TODO
    }

    iterator &operator+=(const int &n) {
      ptr += n;
      return *this;
      // TODO
    }

    iterator &operator-=(const int &n) {
      ptr -= n;
      return *this;
      // TODO
    }
    /**
     * TODO iter++
     */
    iterator operator++(int) {
      ++ptr;
      return iterator(ptr - 1, father);
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() {
      ++ptr;
      return *this;
    }
    /**
     * TODO iter--
     */
    iterator operator--(int) {
      --ptr;
      return iterator(ptr + 1, father);
    }
    /**
     * TODO --iter
     */
    iterator &operator--() {
      --ptr;
      return *this;
    }
    /**
     * TODO *it
     */
    T &operator*() const {

      if (ptr >= father->size() || ptr < 0) {
        throw sjtu::invalid_iterator();
      }

      return father->at(ptr);
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory address).
     */
    bool operator==(const iterator &rhs) const {
      return father == rhs.father && ptr == rhs.ptr;
    }

    bool operator==(const const_iterator &rhs) const {
      return father == rhs.father && ptr == rhs.ptr;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return father != rhs.father || ptr != rhs.ptr;
    }

    bool operator!=(const const_iterator &rhs) const {
      return father != rhs.father || ptr != rhs.ptr;
    }

    // operator const_iterator() { return const_iterator(ptr, father); }
  };
  /**
   * TODO
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;
    friend class iterator;

  private:
    std::size_t ptr;
    const vector<T> *father;

  public:
    const_iterator(const std::size_t &ptr_, const vector<T> *father_)
        : ptr(ptr_), father(father_) {}
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    const_iterator operator+(const int &n) const {
      return const_iterator(ptr + n, father);
      // TODO
    }
    const_iterator operator-(const int &n) const {
      return const_iterator(ptr - n, father);
      // TODO
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const const_iterator &rhs) const {

      if (father != rhs.father) {
        throw sjtu::invalid_iterator();
      }

      return ptr - rhs.ptr;
      // TODO
    }

    const_iterator &operator+=(const int &n) {
      ptr += n;
      return *this;
      // TODO
    }

    const_iterator &operator-=(const int &n) {
      ptr -= n;
      return *this;
      // TODO
    }
    /**
     * TODO iter++
     */
    const_iterator operator++(int) {
      ++ptr;
      return const_iterator(ptr - 1, father);
    }
    /**
     * TODO ++iter
     */
    const_iterator &operator++() {
      ++ptr;
      return *this;
    }
    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      --ptr;
      return const_iterator(ptr + 1, father);
    }
    /**
     * TODO --iter
     */
    const_iterator &operator--() {
      --ptr;
      return *this;
    }
    /*TODO*/
    const T &operator*() const {

      if (ptr >= father->size() || ptr < 0) {
        throw sjtu::invalid_iterator();
      }

      return father->at(ptr);
    }

    // operator iterator() { return iterator(ptr, father); }

    bool operator==(const iterator &rhs) const {
      return father == rhs.father && ptr == rhs.ptr;
    }

    bool operator==(const const_iterator &rhs) const {
      return father == rhs.father && ptr == rhs.ptr;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return father != rhs.father || ptr != rhs.ptr;
    }

    bool operator!=(const const_iterator &rhs) const {
      return father != rhs.father || ptr != rhs.ptr;
    }
  };
  /**
   * TODO Constructs
   * At least two: default constructor, copy constructor
   */
  vector() : size_(0), block_count(1), head(new T *[block_size]) {}

  vector(const vector &other)
      : size_(other.size_), block_count(other.block_count),
        head(new T *[other.block_count * block_size]) {

    for (int i = 0; i != other.size_; ++i) {
      head[i] = new T(other.at(i));
    }
  }

  vector(vector &&other)
      : size_(other.size_), block_count(other.block_count), head(other.head) {
    other.head = nullptr;
    other.size_ = 0;
    other.block_count = 0;
  }
  /**
   * TODO Destructor
   */
  ~vector() {

    if (head == nullptr) {
      return;
    }

    for (int i = 0; i != size_; ++i) {
      delete head[i];
    }

    delete[] head;
  }
  /**
   * TODO Assignment operator
   */
  vector<T> &operator=(const vector<T> &other) {
    if (&other == this) {
      return *this;
    }

    if (head != nullptr) {

      for (int i = 0; i != size_; ++i) {
        delete head[i];
      }
      delete[] head;
    }

    size_ = other.size_;
    block_count = other.block_count;
    head = new T *[other.block_count * block_size];

    for (int i = 0; i != other.size_; ++i) {
      head[i] = new T(other.at(i));
    }

    return *this;
  }

  vector<T> &operator=(vector<T> &&other) {

    if (head != nullptr) {

      for (int i = 0; i != size_; ++i) {
        delete head[i];
      }
      delete[] head;
    }

    head = other.head;
    size_ = other.size_;
    block_count = other.block_count;

    other.head = nullptr;
    other.size_ = 0;
    other.block_count = 0;

    return *this;
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T &at(const size_t &pos) {

    if (pos >= size_ || pos < 0) {
      throw sjtu::index_out_of_bound();
    }

    return *(head[pos]);
  }

  const T &at(const size_t &pos) const {
    if (pos >= size_ || pos < 0) {
      throw sjtu::index_out_of_bound();
    }

    return *(head[pos]);
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attentions
   *   In STL this operator does not check the boundary but I want you to do.
   */
  T &operator[](const size_t &pos) { return at(pos); }

  const T &operator[](const size_t &pos) const { return at(pos); }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T &front() const {

    if (size_ == 0) {
      throw sjtu::container_is_empty();
    }

    return at(0);
  }
  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T &back() const {

    if (size_ == 0) {
      throw sjtu::container_is_empty();
    }

    return at(size_ - 1);
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() { return iterator(0, this); }

  const_iterator cbegin() const { return const_iterator(0, this); }
  /**
   * returns an iterator to the end.
   */
  iterator end() { return iterator(size_, this); }

  const_iterator cend() const { return const_iterator(size_, this); }
  /**
   * checks whether the container is empty
   */
  bool empty() const { return size_ == 0; }
  /**
   * returns the number of elements
   */
  size_t size() const { return size_; }
  /**
   * clears the contents
   */
  void clear() {

    if (head != nullptr) {
      for (int i = 0; i != size_; ++i) {
        delete head[i];
      }
      delete[] head;
    }

    head = new T *[block_size];
    block_count = 1;
    size_ = 0;
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {

    if (pos - begin() > size_) {
      throw sjtu::invalid_iterator();
    }

    if (size_ + 1 == block_count * block_size) {
      multiply();
    }

    for (int i = size_; i != pos - begin(); --i) {
      head[i] = head[i - 1];
    }
    head[pos - begin()] = new T(value);
    ++size_;

    return pos;
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size
   * because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {

    if (ind > size_) {
      throw sjtu::index_out_of_bound();
    }

    if (size_ + 1 == block_count * block_size) {
      multiply();
    }

    for (int i = size_; i != ind; --i) {
      head[i] = head[i - 1];
    }
    head[ind] = new T(value);
    ++size_;

    return iterator(ind, this);
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is
   * returned.
   */
  iterator erase(iterator pos) {

    if (pos - begin() >= size_) {
      throw sjtu::invalid_iterator();
    }

    if (size_ < block_count * block_size / 4) {
      halve();
    }

    delete head[pos - begin()];

    for (int i = pos - begin(); i != size_; ++i) {
      head[i] = head[i + 1];
    }
    // head[size_ - 1] = nullptr;
    --size_;

    return pos;
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {

    if (ind >= size_) {
      throw sjtu::index_out_of_bound();
    }

    if (size_ < block_count * block_size / 4) {
      halve();
    }

    delete head[ind];

    for (int i = ind; i != size_; ++i) {
      head[i] = head[i + 1];
    }
    // head[size_ - 1] = nullptr;
    --size_;

    return iterator(ind, this);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {

    if (size_ + 1 == block_count * block_size) {
      multiply();
    }

    head[size_] = new T(value);
    ++size_;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {

    if (size_ == 0) {
      throw sjtu::container_is_empty();
    }

    if (size_ < block_count * block_size / 4) {
      halve();
    }

    delete head[size_ - 1];
    head[size_ - 1] = nullptr;
    --size_;
  }

private:
  T **head;
  std::size_t size_;
  std::size_t block_count;
  // const static int szT = sizeof(T);
  const static std::size_t block_size = 2 << 9;

  void multiply() {

    block_count *= 2;
    const auto tmp = head;
    head = new T *[block_count * block_size];

    for (int i = 0; i != size_; ++i) {
      head[i] = tmp[i];
    }

    delete[] tmp;
  }

  void halve() {

    if (block_count == 1) {
      return;
    }

    block_count /= 2;
    const auto tmp = head;
    head = new T *[block_count * block_size];

    for (int i = 0; i != size_; ++i) {
      head[i] = tmp[i];
    }

    delete[] tmp;
  }
};

#endif
