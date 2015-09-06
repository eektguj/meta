/**
 * @file analyzer.h
 * @author Sean Massung
 * @author Chase Geigle
 *
 * All files in META are dual-licensed under the MIT and NCSA licenses. For more
 * details, consult the file LICENSE.mit and LICENSE.ncsa in the root of the
 * project.
 */

#ifndef META_ANALYZER_H_
#define META_ANALYZER_H_

#include <stdexcept>
#include <memory>
#include <string>
#include <unordered_map>

namespace cpptoml
{
class table;
}

namespace meta
{

namespace corpus
{
class document;
}

namespace analyzers
{

class token_stream;

template <class T>
class multi_analyzer;

/**
 * An class that provides a framework to produce token counts from documents.
 * All analyzers inherit from this class and (possibly) implement tokenize().
 *
 * The template argument for an analyzer indicates the supported feature
 * value for the analyzer, which is either uint64_t for inverted_index or
 * double for forward_index.
 *
 * When defining your own sublcass of analyzer, you should ensure to
 * subclass from the appropriate type.
 */
template <class T>
class analyzer
{
  public:
    static_assert(std::is_same<T, uint64_t>::value
                      || std::is_same<T, double>::value,
                  "analyzers can only produce unsigned integer or real valued "
                  "feature values");

    using base_type = analyzer;
    using feature_value_type = T;

    using feature_map = std::unordered_map<std::string, T>;

    /**
     * A default virtual destructor.
     */
    virtual ~analyzer() = default;

    /**
     * Tokenizes a document.
     * @param doc The document to be tokenized
     * @return a feature_map that maps the observed features to their
     *  counts in the document
     */
    feature_map analyze(const corpus::document& doc);

    /**
     * Clones this analyzer.
     */
    virtual std::unique_ptr<analyzer> clone() const = 0;

    friend multi_analyzer<T>;

  private:
    /**
     * The tokenization function that actually does the heavy lifting. This
     * should be overridden in derived classes.
     *
     * @param doc The document to be tokenized
     * @param counts The feature_map to place observed feature counts into
     */
    virtual void tokenize(const corpus::document& doc, feature_map& counts) = 0;
};

/**
 * Basic exception for analyzer interactions.
 */
class analyzer_exception : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @param config The config group used to create the analyzer from
 * @return an analyzer as specified by a config object
 */
template <class T>
std::unique_ptr<analyzer<T>> load(const cpptoml::table& config);

extern template std::unique_ptr<analyzer<uint64_t>>
    load(const cpptoml::table& config);
extern template std::unique_ptr<analyzer<double>>
    load(const cpptoml::table& config);

/**
 * @param config The config group used to create the analyzer from
 * @return the default filter chain for this version of MeTA,
 * based on a config object
 */
std::unique_ptr<token_stream>
    default_filter_chain(const cpptoml::table& config);

/**
 * @param config The config group used to create the analyzer from
 * @return the default filter chain for unigram words for this version
 * of MeTA, based on a config object
 */
std::unique_ptr<token_stream>
    default_unigram_chain(const cpptoml::table& config);

/**
 * @param global The original config object with all parameters
 * @param config The config group used to create the filters from
 * @return a filter chain as specified by a config object
 */
std::unique_ptr<token_stream> load_filters(const cpptoml::table& global,
                                           const cpptoml::table& config);

/**
 * @param src The token stream that will feed into this filter
 * @param config The config group used to create the filter from
 * @return a single filter specified by a config object
 */
std::unique_ptr<token_stream> load_filter(std::unique_ptr<token_stream> src,
                                          const cpptoml::table& config);

/**
 * @param doc The document to get content for
 * @return the contents of the document, as a string
 */
std::string get_content(const corpus::document& doc);
}
}
#endif
