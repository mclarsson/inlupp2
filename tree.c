#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tree.h"

typedef struct node node_t;

struct tree {
  node_t *top;
  int size;
};

struct node {
  K key;
  T element;
  node_t *left;
  node_t *right;
};

/// Whether or not two keys matches.
///
/// \param k1 First key
/// \param k2 Second key
/// \returns If they match
bool key_matches(K k1, K k2)
{
  return strcmp(k1, k2) == 0;
}

/// Creates a new tree
///
/// \returns: empty tree
tree_t *tree_new()
{
  return calloc(1, sizeof(tree_t));
}

/// Frees node and subtrees
///
/// \param node Node to start remove at
void free_branches(node_t *node, tree_action cleanup)
{
  if (node->left != NULL)
    {
      free_branches(node->left, cleanup);
    }

  if (node->right != NULL)
    {
      free_branches(node->right, cleanup);
    }

  cleanup(node->key, node->element);
}

/// Remove a tree along with all T elements.
///
/// \param tree the tree
/// \param cleanup a function that takes a key and element as
///        argument, to be used to free memory. If this param is 
///        NULL, no cleanup of keys or elements will happen.
void tree_delete(tree_t *tree, tree_action cleanup)
{
  if (tree->top != NULL)
    {
      free_branches(tree->top, cleanup);
    }

  free(tree);
}

/// Get the size of the tree 
///
/// \returns: the number of nodes in the tree
int tree_size(tree_t *tree)
{
  return tree->size;
}

/// Return the biggest of two numbers
///
/// \param a First number
/// \param b Second number
/// \returns Biggest number
int max(int a, int b)
{
  return a > b ? a : b;
}

/// Count max node depth
///
/// \param node current node
int count_depth(node_t *node)
{
  if (node->left == NULL && node->right == NULL)
    {
      return 1;
    }
  else
    {
      int left  = node->left  == NULL ? 1 : 1 + count_depth(node->left);
      int right = node->right == NULL ? 1 : 1 + count_depth(node->right);
      return max(left, right);
    }
}

/// Get the depth of the tree 
///
/// \returns: the depth of the deepest subtree
int tree_depth(tree_t *tree)
{
  return tree->top == NULL ? 0 : count_depth(tree->top);
}

/// Help function for node insertion, whether or not k1 is alphabetically before k2
///
/// \param k1 First key
/// \param k2 Second key
bool move_right(K k1, K k2)
{
  // strcmp return negative when k1 comes before k2
  return strcmp(k1, k2) < 0;
}

/// Finds correct parent for node.
///
/// \param parent node to start the search at.
/// \param insert node to insert.
bool insert_node(node_t *parent, node_t *insert)
{
  if (key_matches(parent->key, insert->key))
    {
      return false;
    }
  else if (move_right(parent->key, insert->key))
    {
      if (parent->right == NULL)
	{
	  parent->right = insert;
	  return true;
	}
      else
	{
	  return insert_node(parent->right, insert);
	}
    }
  else
    {
      if (parent->left == NULL)
	{
	  parent->left = insert;
	  return true;
	}
      else
	{
	  return insert_node(parent->left, insert);
	}
    }
}

/// Insert element into the tree. Returns false if the key is already used.
///
/// \param tree pointer to the tree
/// \param key the key of element to be appended
/// \param elem the element 
/// \returns: true if successful, else false
bool tree_insert(tree_t *tree, K key, T elem)
{ 
  node_t *new = calloc(1, sizeof(node_t));
  new->key = strdup(key);
  new->element = elem;
  
  if (tree->top == NULL)
    {
      tree->size = 1;
      tree->top = new;
      return true;
    }
  else if (insert_node(tree->top, new))
    {
      tree->size++;
      return true;
    }

  return false;
}

/// Helper for tree_has_key TODO
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
bool look_for_key(node_t *node, K key)
{
  if (key_matches(node->key, key))
    {
      return true;
    }
  else if (move_right(node->key, key) && node->right != NULL)
    {
      return look_for_key(node->right, key);
    }
  else if (node->left != NULL)
    {
      return look_for_key(node->right, key);
    }
  else
    {
      return false;
    }
}

/// Checks whether a key is used in a tree
///
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
bool tree_has_key(tree_t *tree, K key)
{
  return tree->top == NULL ? false : look_for_key(tree->top, key);
}

/// Helper function for tree_get
///
/// \param node Node to search at.
/// \param key Key to find.
/// \returns Matching node.
T find_element(node_t *node, K key)
{
  if (key_matches(node->key, key))
    {
      return node->element;
    }
  else if (move_right(node->key, key))
    {
      return find_element(node->right, key);
    }
  else
    {
      return find_element(node->left, key);
    }
}

/// Returns the element for a given key in tree.
/// (The implementation may assume that the key exists.)
/// 
/// \param tree pointer to the tree
/// \param key the key of elem to be removed
/// \returns: true if key is a key in tree
T tree_get(tree_t *tree, K key)
{
  return find_element(tree->top, key);
}

/// Collects elements in tree in ascending order into array.
///
/// \param node Current node to collect.
/// \param elements Array to store elements in.
/// \param cur_index Current index to store element in array at.
/// \returns next available index in array.
int collect_elements_old(node_t *node, T *elements, int cur_index)
{
  if (node->left == NULL && node->right == NULL)
    {
      // End of branch
      elements[cur_index] = node->element;
      return cur_index + 1;
    }
  else
    {
      int new_index = cur_index;
      if (node->left != NULL)
	{
	  // Collect nodes in left branch first
	  new_index = collect_elements_old(node->left, elements, cur_index);
	}

      // Add this node after every node on the left branch has been added
      elements[new_index] = node->element;
      
      if (node->right == NULL)
	{
	  // Return new index to parent node when done with this node
	  return new_index + 1;
	}
      else
	{
	  // Collect all nodes in right branch before parent node is collected
	  return collect_elements_old(node->right, elements, new_index + 1);
	}
    }
}

T *collect_elements(node_t *node, T *elements)
{
  // End of branch
  if (node->left == NULL && node->right == NULL)
    {
      // Increment AFTER this node has been added
      *elements = node->element;
      return ++elements;
    }
  else
    {
      // Collect nodes in left branch first
      if (node->left != NULL) elements = collect_elements(node->left, elements);

      // Increment AFTER this node has been added
      *elements = node->element;

      // Collect all nodes in right branch after parent node is collected
      if (node->right != NULL) return collect_elements(node->right, ++elements);

      // Move back up tree
      return ++elements;
    }
}

/// Returns an array holding all the elements in the tree
/// in ascending order of their keys (which are not part
/// of the value).
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() elements
T *tree_elements_old(tree_t *tree)
{
  int size = tree_size(tree);
  T *elements = calloc(size, sizeof(T));
  if (size > 0)
    {
      collect_elements_old(tree->top, elements, 0);
    }
  return elements;
}

T *tree_elements(tree_t *tree)
{
  int size = tree_size(tree);
  T *elements = calloc(size, sizeof(T));
  if (size > 0)
    {
      collect_elements(tree->top, elements);
    }
  return elements;
}

/// Collects elements in tree in ascending order into array.
///
/// \param node Current node to collect.
/// \param elements Array to store elements in.
/// \param cur_index Current index to store element in array at.
/// \returns next available index in array.
K *collect_keys(node_t *node, K *keys)
{
  if (node->left == NULL && node->right == NULL)
    {
      // Move back up tree when end is reached
      *keys = node->key;
      return ++keys;
    }
  else
    {
      // Collect nodes in left branch first
      if (node->left != NULL) keys = collect_keys(node->left, keys);

      // Add current node
      *keys = node->key;
      
      // Collect all nodes in right branch after parent node is collected
      if (node->right != NULL) return collect_keys(node->right, ++keys);

      // Move back up tree
      return ++keys;
    }
}


/// Returns an array holding all the keys in the tree
/// in ascending order.
///
/// \param tree pointer to the tree
/// \returns: array of tree_size() keys
K *tree_keys(tree_t *tree)
{
  int size = tree_size(tree);
  K *keys = calloc(size, sizeof(K));
  if (size > 0)
    {
      //int index = -1;
      collect_keys(tree->top, keys);
    }
  
  return keys;
}
