/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A class for non-null strong pointers to reference-counted objects. */

#ifndef mozilla_dom_OwningNonNull_h
#define mozilla_dom_OwningNonNull_h

#include "nsAutoPtr.h"
#include "nsCycleCollectionNoteChild.h"

namespace mozilla {
namespace dom {

template<class T>
class OwningNonNull
{
public:
  OwningNonNull() {}

  MOZ_IMPLICIT OwningNonNull(T& aValue)
  {
    init(&aValue);
  }

  template<class U>
  MOZ_IMPLICIT OwningNonNull(already_AddRefed<U>&& aValue)
  {
    init(aValue.take());
  }

  // This is no worse than get() in terms of const handling.
  operator T&() const
  {
    MOZ_ASSERT(mInited);
    MOZ_ASSERT(mPtr, "OwningNonNull<T> was set to null");
    return *mPtr;
  }

  operator T*() const
  {
    MOZ_ASSERT(mInited);
    MOZ_ASSERT(mPtr, "OwningNonNull<T> was set to null");
    return mPtr;
  }

  // Conversion to bool is always true, so delete to catch errors
  explicit operator bool() const = delete;

  T*
  operator->() const
  {
    MOZ_ASSERT(mInited);
    MOZ_ASSERT(mPtr, "OwningNonNull<T> was set to null");
    return mPtr;
  }

  OwningNonNull<T>&
  operator=(T* aValue)
  {
    init(aValue);
    return *this;
  }

  OwningNonNull<T>&
  operator=(T& aValue)
  {
    init(&aValue);
    return *this;
  }

  OwningNonNull<T>&
  operator=(const already_AddRefed<T>& aValue)
  {
    init(aValue);
    return *this;
  }

  // Don't allow assigning nullptr, it makes no sense
  void operator=(decltype(nullptr)) = delete;

  already_AddRefed<T> forget()
  {
#ifdef DEBUG
    mInited = false;
#endif
    return mPtr.forget();
  }

  // Make us work with smart pointer helpers that expect a get().
  T* get() const
  {
    MOZ_ASSERT(mInited);
    MOZ_ASSERT(mPtr);
    return mPtr;
  }

  template<typename U>
  void swap(U& aOther)
  {
    mPtr.swap(aOther);
  }

protected:
  template<typename U>
  void init(U aValue)
  {
    mPtr = aValue;
    MOZ_ASSERT(mPtr);
#ifdef DEBUG
    mInited = true;
#endif
  }

  nsRefPtr<T> mPtr;
#ifdef DEBUG
  bool mInited = false;
#endif
};

template <typename T>
inline void
ImplCycleCollectionTraverse(nsCycleCollectionTraversalCallback& aCallback,
                            OwningNonNull<T>& aField,
                            const char* aName,
                            uint32_t aFlags = 0)
{
  CycleCollectionNoteChild(aCallback, aField.get(), aName, aFlags);
}

} // namespace dom
} // namespace mozilla

// Declared in nsCOMPtr.h
template<class T> template<class U>
nsCOMPtr<T>::nsCOMPtr(const mozilla::dom::OwningNonNull<U>& aOther)
  : nsCOMPtr(aOther.get())
{}

template<class T> template<class U>
nsCOMPtr<T>&
nsCOMPtr<T>::operator=(const mozilla::dom::OwningNonNull<U>& aOther)
{
  return operator=(aOther.get());
}

// Declared in mozilla/nsRefPtr.h
template<class T> template<class U>
nsRefPtr<T>::nsRefPtr(const mozilla::dom::OwningNonNull<U>& aOther)
  : nsRefPtr(aOther.get())
{}

template<class T> template<class U>
nsRefPtr<T>&
nsRefPtr<T>::operator=(const mozilla::dom::OwningNonNull<U>& aOther)
{
  return operator=(aOther.get());
}

#endif // mozilla_dom_OwningNonNull_h
