/* 
 *  Copyright (c) 2010 Daisuke Okanohara
  * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#ifndef UX_HPP__
#define UX_HPP__

#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>
#include "bitVec.hpp"

namespace ux_tool{

typedef uint32_t id_t;

enum {
  NOTFOUND      = 0xFFFFFFFFU,
  LIMIT_DEFAULT = 0xFFFFFFFFU
};


/**
 * Succinct Trie Data structure
 */
class UX{
public:
  enum {
    FILE_OPEN_ERROR  = 1,
    FILE_WRITE_ERROR = 2,
    FILE_READ_ERROR  = 3,
    SAVE_ERROR       = 4,
    LOAD_ERROR       = 5
  };

public:
  /**
   * Constructor
   */
  UX();

  /**
   * Destructor
   */
  ~UX();

  /**
   * Build a dictionary from wordList
   * @param wordList input word list
   * @param isTailUX use tail compression. 
   */
  void build(std::vector<std::string>& wordList, bool isTailUX = true);
  
  /**
   * Save the dictionary in a file
   * @param indexName The file name
   * @return 0 on success, -1 on failure
   */
  int save(const char* indexName);

  /**
   * Load the dicitonary from a file
   * @param indexName The filename
   * @return 0 on success, -1 on failure
   */
  int load(const char* indexName);

  /**
   * Save the dictionary in ostream
   * @param os The ostream as an output 
   * @return 0 on success, -1 on failure
   */
  int save(std::ostream& os);

  /**
   * Load the dictionary from istream
   * @param is The istream as an input
   * @return 0 on success, -1 on failure
   */
  int load(std::istream& is);

  /**
   * Return the longest word that matches the prefix of the query in the dictionary
   * @param str the query
   * @param len the length of the query
   * @param retLen The length of the matched word in the dictionary 
   * @return The ID of the matched word or NOTFOUND if no word is matched
   */
  id_t prefixSearch(const char* str, size_t len, size_t& retLen) const;

  /** 
   * Return the all words that match the prefix of the query in the dictionary
   * @param str the query
   * @param len the length of the query
   * @param retIDs The IDs of the matched words
   * @param limit The maximum number of matched words
   * @return The number of matched words
   */
  size_t commonPrefixSearch(const char* str, size_t len, std::vector<id_t>& retIDs, 
			    size_t limit = LIMIT_DEFAULT) const;

  /** 
   * Return the all words whose their prefixes  match the query 
   * @param str the query
   * @param len the length of the query
   * @param The IDs of the matched words
   * @param limit The maximum number of matched words
   * @return The number of matched words
   */
  size_t predictiveSearch(const char* str, size_t len, std::vector<id_t>& retIDs, 
			  size_t limit = LIMIT_DEFAULT) const;
  
  /**
   * Return the word for the given ID
   * @param id The ID of the word
   * @param ret The word for the given ID or empty if such ID does not exist
   */ 
  void decode(const id_t id, std::string& ret) const;

  /**
   * Return the word for the given ID
   * @param id The ID of the word
   * @return The word for the given ID or empty if such ID does not exist
   */ 
  std::string decode(const id_t id) const;
  
  /**
   * Return the number of words in the dictionary
   * @return the number of words in the dictionary
   */
  size_t getKeyNum() const;

  /**
   * Report the error message for the error ID
   * @param error The error ID
   * @return The error message
   */
  static std::string what(const int error);
  
  /*
   * Get the allocated memory size
   * @return The size of the allocated memory
   */
  size_t getAllocSize() const;
  
  /*
   * Report the statistics of the memory allocation
   * @param allocSize The initial overhead size
   */
  void allocStat(size_t allocSize) const;

  /*
   * Report the dicionary for the wordList
   * @param wordList The word list to be examined
   */
  void stat(std::vector<std::string>& wordList) const;

private:
  static size_t log2(size_t x);

  bool isLeaf(const uint32_t pos) const;
  void getChild(const uint8_t c, uint32_t& pos, uint32_t& zeros) const;
  void getParent(uint8_t& c, uint32_t& pos, uint32_t& zeros) const;
  void buildTailUX();
  
  void traverse(const char* str, size_t len, size_t& retLen, std::vector<id_t>& retIDs, 
		size_t limit) const;

  void enumerateAll(const uint32_t pos, const uint32_t zeros, std::vector<id_t>& retIDs, size_t limit) const;
  bool tailMatch(const char* str, size_t len, size_t depth,
		 const uint32_t tailID, size_t& retLen) const;
  std::string getTail(const uint32_t i) const;

  RSDic loud_;
  RSDic terminal_;
  RSDic tail_;

  std::vector<std::string> vtails_;
  UX* vtailux_;

  std::vector<uint8_t> edges_;
  BitVec tailIDs_;
  size_t tailIDLen_;

  size_t keyNum_;

  bool isReady_;
};

}

#endif // UX_HPP__
