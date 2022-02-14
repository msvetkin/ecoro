// Copyright 2022 - present, Mikhail Svetkin
// All rights reserved.
//
// For the license information refer to LICENSE

#ifndef ECORO_DETAIL_INTRUSIVE_LIST_HPP
#define ECORO_DETAIL_INTRUSIVE_LIST_HPP

#include <iterator>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace ecoro::detail::intrusive {

template<typename T>
struct list_node {
  using value_type = T;

  auto get() noexcept {
    return reinterpret_cast<T *>(this);
  }

  list_node *next{nullptr};
  list_node *previous{nullptr};
};

template<typename Node>
struct list_iterator {
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = typename Node::value_type;
  using pointer           = value_type*;
  using reference         = value_type&;

  explicit list_iterator(Node *const node) noexcept
    : node(node) {
  }

  reference operator*() const noexcept {
    return *node->get();
  }

  pointer operator->() const noexcept {
    return node->get();
  }

  list_iterator& operator++() noexcept {
    node = node->next;
    return *this;
  }

  list_iterator& operator--() noexcept {
    node = node->previous;
    return *this;
  }

  friend bool operator!=(const list_iterator &left,
                         const list_iterator &right) noexcept {
    return left.node != right.node;
  };

  friend bool operator==(const list_iterator &left,
                         const list_iterator &right) noexcept {
    return left.node == right.node;
  };

  Node *node{nullptr};
};

template<typename T>
class list {
 public:
  using value_type = T;
  using pointer_type = std::add_pointer_t<value_type>;
  using reference = std::add_lvalue_reference_t<value_type>;
  using const_reference = std::add_const_t<reference>;
  using size_type = std::size_t;

  using node_type = list_node<value_type>;
  using iterator = list_iterator<node_type>;

  list() noexcept {
    clear();
  }

  auto begin() noexcept {
    return iterator{root_.next};
  };

  auto end() noexcept {
    return iterator{&root_};
  };

  bool empty() const noexcept {
    return root_.next == &root_;
  }

  iterator insert(iterator pos, const reference v) noexcept {
    auto *new_node = to_node(v);
    iterator prev{pos.node->previous};

    new_node->next = std::exchange(prev.node->next, new_node);
    new_node->previous = std::exchange(pos.node->previous, new_node);

    return iterator{new_node};
  }

  void push_back(const reference v) noexcept {
    insert(end(), v);
  }

  iterator erase(iterator pos) noexcept {
    iterator prev{pos.node->previous};
    iterator next{pos.node->next};

    prev.node->next = std::exchange(pos.node->next, nullptr);
    next.node->previous = std::exchange(pos.node->previous, nullptr);
    return next;
  }

  void clear() noexcept {
    root_.next = root_.previous = &root_;
  }

  static auto iterator_to(const reference v) noexcept {
    return iterator{to_node(v)};
  }

 protected:
  static node_type *to_node(const reference v) noexcept {
    return reinterpret_cast<node_type *>(&v);
  }

 private:
  node_type root_;
};

}  // namespace ecoro::detail

#endif  // ECORO_DETAIL_INTRUSIVE_LIST_HPP
