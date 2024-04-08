#pragma once
#include "array_ptr.h"
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stdexcept>

class ReserveProxyObj {
public:
  explicit ReserveProxyObj(size_t capacity_to_reserve)
      : capacity_(capacity_to_reserve) {}

  size_t GetCapacity() { return capacity_; }

private:
  size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
  return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type> class SimpleVector {
public:
  using Iterator = Type *;
  using ConstIterator = const Type *;

  SimpleVector() noexcept = default;

  SimpleVector(ReserveProxyObj reserve) { Reserve(reserve.GetCapacity()); }

  SimpleVector(const SimpleVector &other)
      : size_(other.size_), capacity_(other.capacity_), raw_ptr_(other.size_) {
    std::copy(other.begin(), other.end(), begin());
  }

  explicit SimpleVector(size_t size)
      : size_(size), capacity_(size), raw_ptr_(size) {
    std::fill(begin(), end(), Type{});
  }

  SimpleVector(size_t size, const Type &value)
      : size_(size), capacity_(size), raw_ptr_(size) {
    std::fill(begin(), end(), value);
  }

  SimpleVector(SimpleVector<Type> &&other) noexcept { swap(other); }

  SimpleVector(std::initializer_list<Type> init)
      : size_(init.size()), capacity_(init.size()), raw_ptr_(init.size()) {
    std::copy(init.begin(), init.end(), begin());
  }

  SimpleVector &operator=(SimpleVector &&rhs) {
    if (this != &rhs) {
      SimpleVector<Type> tmp(std::move(rhs));
      swap(tmp);
    }
    return *this;
  };

  SimpleVector &operator=(const SimpleVector &rhs) {
    if (this != &rhs) {
      SimpleVector<Type> tmp(rhs);
      swap(tmp);
    }
    return *this;
  }

  size_t GetSize() const noexcept { return size_; }

  size_t GetCapacity() const noexcept { return capacity_; }

  bool IsEmpty() const noexcept { 
    return !size_; 
    }


  Type &operator[](std::size_t index) noexcept {
    assert(index < size_);
    return raw_ptr_[index];
  }

  const Type &operator[](std::size_t index) const noexcept {
    assert(index < size_);
    return raw_ptr_[index];
  }

  Type &At(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("out of range");
    }
    return raw_ptr_[index];
  }

  const Type &At(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("out of range");
    }
    return raw_ptr_[index];
  }

  void Clear() noexcept { size_ = 0; }

  void Resize(size_t new_size) {
    if (new_size <= capacity_) {
      std::fill(begin() + size_, begin() + new_size, Type());

    } else if (new_size > capacity_) {
      Reserve(std::max(new_size, 2 * capacity_));
      std::fill(begin() + size_, begin() + new_size, Type());
    }
    size_ = new_size;
  }

  void PushBack(Type &&item) {

    if (size_ < capacity_) {
      raw_ptr_[size_] = std::move(item);
    } else {
      Reserve(std::max(static_cast<size_t>(1), 2 * capacity_));
      raw_ptr_[size_] = std::move(item);
    }
    ++size_;
  }

  void PushBack(const Type &item) {
    if (size_ < capacity_) {
      raw_ptr_[size_] = item;
    } else {
      Reserve(std::max(static_cast<size_t>(1), 2 * capacity_));
      raw_ptr_[size_] = item;
    }
    ++size_;
  }

  Iterator Insert(ConstIterator pos, Type &&value) {
    assert(pos > begin() || pos < end());
    size_t number_pos = VectorMove(pos);
    raw_ptr_[number_pos] = std::move(value);
    return &raw_ptr_[number_pos];
  }

  Iterator Insert(ConstIterator pos, const Type &value) {
    assert(pos > begin() || pos < end());
    size_t number_pos = VectorMove(pos);
    raw_ptr_[number_pos] = value;
    return &raw_ptr_[number_pos];
  }

  void PopBack() noexcept {
    assert(IsEmpty());
    --size_;
  }

  Iterator Erase(ConstIterator pos) {
    assert(pos > begin() || pos < end());
    Iterator it = &raw_ptr_[std::distance<ConstIterator>(cbegin(), pos)];
    std::move(it + 1, end(), it);
    --size_;
    return it;
  }

  void Reserve(size_t new_capacity) {
    if (new_capacity >= capacity_) {
      ArrayPtr<Type> new_items(new_capacity);
      // std::fill(new_items.Get(), new_items.Get() + new_capacity, Type());
      std::move(begin(), end(), new_items.Get());
      raw_ptr_.swap(new_items);
      capacity_ = new_capacity;
    }
  }

  void swap(SimpleVector &other) noexcept {
    raw_ptr_.swap(other.raw_ptr_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
  }

  Iterator begin() noexcept { return Iterator{raw_ptr_.Get()}; }

  Iterator end() noexcept { return Iterator{raw_ptr_.Get() + size_}; }

  ConstIterator begin() const noexcept { return cbegin(); }

  ConstIterator end() const noexcept { return cend(); }

  ConstIterator cbegin() const noexcept {
    return ConstIterator{raw_ptr_.Get()};
  }

  ConstIterator cend() const noexcept {
    return ConstIterator{cbegin() + size_};
  }

private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  ArrayPtr<Type> raw_ptr_;

  size_t VectorMove(ConstIterator pos) {
    size_t number_pos = std::distance<ConstIterator>(cbegin(), pos);
    if (size_ < capacity_) {
      if (pos == end()) {
        assert(pos == end());
      }
      std::move_backward(&raw_ptr_[number_pos], end(), &raw_ptr_[size_ + 1]);
    } else {
      if (capacity_ == 0) {
        Reserve(1);
      } else {
        Reserve(2 * capacity_);
        std::move_backward(&raw_ptr_[number_pos], end(), &raw_ptr_[size_ + 1]);
      }
    }
    ++size_;
    return number_pos;
  }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  if (lhs.GetSize() != rhs.GetSize()) {
    return false;
  }
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  bool value = (!(lhs == rhs));
  return value;
}

template <typename Type>
inline bool operator<(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  return !(rhs < lhs);
  return true;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs) {
  return rhs < lhs;
  return true;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs) {
  return !(lhs < rhs);
}
