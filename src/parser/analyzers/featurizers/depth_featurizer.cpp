#include "parser/analyzers/featurizers/depth_featurizer.h"
#include "parser/trees/visitors/visitor.h"
#include "parser/trees/internal_node.h"
#include "parser/trees/leaf_node.h"

namespace meta
{
namespace analyzers
{

template <class T>
const std::string depth_featurizer<T>::id = "depth";

namespace
{
class height_visitor : public parser::const_visitor<size_t>
{
  public:
    size_t operator()(const parser::internal_node& in) override
    {
        size_t max_height = 0;
        in.each_child([&](const parser::node* child)
                      {
                          max_height
                              = std::max(max_height, child->accept(*this));
                      });
        return max_height + 1;
    }

    size_t operator()(const parser::leaf_node&) override
    {
        // leaf nodes are pre-terminals
        return 1;
    }
};
}

template <class T>
void depth_featurizer<T>::tree_tokenize(const parser::parse_tree& tree,
                                        feature_map& counts) const
{
    height_visitor vtor;
    auto rep = "depth-" + std::to_string(tree.visit(vtor));
    counts[rep] += 1;
}

template class depth_featurizer<uint64_t>;
template class depth_featurizer<double>;
}
}
