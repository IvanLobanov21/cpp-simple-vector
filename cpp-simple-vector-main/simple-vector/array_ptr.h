#pragma once
#include <utility>

template <typename Type> class ArrayPtr {
public:
  // Инициализирует ArrayPtr нулевым указателем
  ArrayPtr() = default;

  // Создаёт в куче массив из size элементов типа Type.
  // Если size == 0, поле raw_ptr_ должно быть равно nullptr
  explicit ArrayPtr(std::size_t size)
      : raw_ptr_(size != 0 ? new Type[size] : nullptr) {}

  // Конструктор из сырого указателя, хранящего адрес массива в куче либо
  // nullptr
  explicit ArrayPtr(Type *raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

  ArrayPtr(ArrayPtr &&other) { raw_ptr_ = std::move(other.raw_ptr_); }

  // Запрещаем копирование
  ArrayPtr(const ArrayPtr &) = delete;

  ~ArrayPtr() {
    // Напишите деструктор самостоятельно
    delete[] raw_ptr_;
  }

  // Запрещаем присваивание
  ArrayPtr &operator=(const ArrayPtr &) = delete;

  ArrayPtr &operator=(ArrayPtr &&other) {
    raw_ptr_ = std::move(other.raw_ptr_);
    return *this;
  }

  // Прекращает владением массивом в памяти, возвращает значение адреса массива
  // После вызова метода указатель на массив должен обнулиться
  [[nodiscard]] Type *Release() noexcept {
    // Заглушка. Реализуйте метод самостоятельно
    auto result = raw_ptr_;
    raw_ptr_ = nullptr;
    return result;
  }

  // Возвращает ссылку на элемент массива с индексом index
  Type &operator[](std::size_t index) noexcept { return raw_ptr_[index]; }

  // Возвращает константную ссылку на элемент массива с индексом index
  const Type &operator[](std::size_t index) const noexcept {
    // Реализуйте операцию самостоятельно
    return raw_ptr_[index];
  }

  // Возвращает true, если указатель ненулевой, и false в противном случае
  explicit operator bool() const {
    // Заглушка. Реализуйте операцию самостоятельно
    return raw_ptr_;
  }

  // Возвращает значение сырого указателя, хранящего адрес начала массива
  Type *Get() const noexcept {
    // Заглушка. Реализуйте метод самостоятельно
    return raw_ptr_;
  }

  // Обменивается значениям указателя на массив с объектом other
  void swap(ArrayPtr &other) noexcept {
    // Реализуйте метод самостоятельно
    std::swap(raw_ptr_, other.raw_ptr_);
  }

private:
  Type *raw_ptr_ = nullptr;
};
