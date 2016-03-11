<?php

/**
 * Autocomplete callback for nodes by title.
 *
 * Searches for a node by title, but then identifies it by nid, so the actual
 * returned value can be used later by the form.
 *
 * The returned $matches array has
 * - key: The title, with the identifying nid in brackets, like "Some node
 *   title [3325]"
 * - value: the title which will is displayed in the autocomplete pulldown.
 *
 * Note that we must use a key style that can be parsed successfully and
 * unambiguously. For example, if we might have node titles that could have
 * [3325] in them, then we'd have to use a more restrictive token.
 *
 * @param string $string
 *   The string that will be searched.
 */
function nice_blocks_autocomplete($string = "") {
  $matches = array();
  if ($string && is_numeric($string)) {
    $node = node_load($string);
    if ($node) {
      $matches[$node->title . " [$node->nid]"] = check_plain($node->title);
    }
  }
  elseif ($string) {
    $result = db_select('node')
      ->fields('node', array('nid', 'title'))
      ->condition('title', db_like($string) . '%', 'LIKE')
      ->range(0, 10)
      ->execute();
    foreach ($result as $node) {
      $matches[$node->title . " [$node->nid]"] = check_plain($node->title);
    }
  }

  backdrop_json_output($matches);
}

/**
 * Node title validation handler.
 *
 * Validate handler to convert our string like "Some node title [3325]" into a
 * nid.
 *
 * In case the user did not actually use the autocomplete or have a valid string
 * there, we'll try to look up a result anyway giving it our best guess.
 *
 * Since the user chose a unique node, we must now use the same one in our
 * submit handler, which means we need to look in the string for the nid.
 *
 * @param $string
 *   The string to validate.
 * @return $nid
 *   A node ID if matched, or NULL if no match.
 *
 * @see node_autocomplete()
 */
function nice_blocks_autocomplete_validate($string) {
  $matches = array();
  $nid = 0;

  // This preg_match() looks for the last pattern like [33334] and if found
  // extracts the numeric portion.
  $result = preg_match('/\[([0-9]+)\]$/', $string, $matches);
  if ($result > 0) {
    // If $result is nonzero, we found a match and can use it as the index into
    // $matches.
    $nid = $matches[$result];
    // Verify that it's a valid nid.
    $node = node_load($nid);
    if (empty($node)) {
      return NULL;
    }
  }
  // If the input was numeric, check that it matches a node.
  elseif (is_numeric($string) && node_load($string)) {
    $nid = (int) $string;
  }
  // Check that the user may have directly entered a node title.
  else {
    $nid = db_select('node')
      ->fields('node', array('nid'))
      ->condition('title', db_like($string) . '%', 'LIKE')
      ->range(0, 1)
      ->execute()
      ->fetchField();
  }

  return (!empty($nid)) ? $nid : NULL;
}
/*
 * helper for reference_dialog string
 *
function _nice_blocks_extract_id($string) {
  $matches = array();
  $nid = 0;

  if (is_numeric($string)) {
    $nid = $string;
  }
  else {
    preg_match('/^(?:\s*|(.*) )?\[\s*nid\s*:\s*(\d+)\s*\]$/', $string, $matches);
    if (!empty($matches)) {
      // Explicit nid. Check that the 'title' part matches the actual title for
      // the nid.
      list(, $title, $nid) = $matches;
      if (!empty($nid)) {
        // Verify that it's a valid nid.
        $node = node_load($nid);
        if (empty($node)) {
          return NULL;
        }
      }
    }
  }

  return (!empty($nid)) ? $nid : NULL;
}
/* */
