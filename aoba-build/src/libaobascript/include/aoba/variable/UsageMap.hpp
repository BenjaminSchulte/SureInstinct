#pragma once

#include <QVector>
#include <QList>

namespace Aoba {

struct UsageMapSection {
  enum Usage {
    FREE,
    UNKNOWN,
    BLOCKED
  };

  UsageMapSection() {}
  UsageMapSection(Usage usage, const uint64_t &start, const uint64_t &size) : usage(usage), start(start), size(size) {}

  inline bool isBlocked() const { return usage == BLOCKED; }
  inline bool isFree() const { return usage == FREE; }
  inline bool isUnknown() const { return usage == UNKNOWN; }

  inline void block() { usage = BLOCKED; }
  inline void unknown() { usage = UNKNOWN; }
  inline void free() { usage = FREE; }

  inline uint64_t end() const { return start+size-1; }

  Usage usage;
  uint64_t start;
  uint64_t size;
};

typedef QList<UsageMapSection> UsageMapSectionList;

struct UsageMapAddressRange {
  UsageMapAddressRange() : from(0), to(0) {}
  UsageMapAddressRange(const uint64_t &from, const uint64_t &to) : from(from), to(to) {}

  uint64_t from;
  uint64_t to;
};

typedef QVector<UsageMapAddressRange> UsageMapAddressRanges;

class UsageMap {  
public:
  UsageMap();
  UsageMap(const uint64_t &offset, const uint64_t &size, UsageMapSection::Usage usage=UsageMapSection::FREE);

  static UsageMap *clone(UsageMap *);

  inline const UsageMapSectionList &all() const { return map; }

  bool block(const uint64_t &offset, const uint64_t &size);
  bool free(const uint64_t &offset, const uint64_t &size);
  bool set(const uint64_t &offset, const uint64_t &size, UsageMapSection::Usage usage);

  void block(const UsageMap &other);
  void free(const UsageMap &other);
  void set(const UsageMap &other, UsageMapSection::Usage where, UsageMapSection::Usage to);

  UsageMapAddressRanges findFreeAddresses(const uint64_t &from, const uint64_t &to, const uint64_t &size);

  inline uint64_t getOffset() const { return offset; }
  inline uint64_t getSize() const { return size; }
  uint64_t getSizeOf(UsageMapSection::Usage) const;

protected:
  UsageMapSectionList::iterator find(const uint64_t &offset);

  UsageMapSectionList map;

  uint64_t offset;
  uint64_t size;
};

}
