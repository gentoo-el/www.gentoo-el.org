<?php
// $Id: hooks.php,v 1.7 2010/10/28 20:31:48 jcmc Exp $

/**
 * @file
 * This file provide additional documentation for doxygen.
 * These are the hooks that are invoked by the Gallery Assist core.
 *
 * Core hooks are typically called in all modules at once using
 * module_invoke_all().
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * It extend the output from a Gallery Assist image in the preview
 * with added code and implemented functionalities by other modules.
 *
 * @param string $op
 *   Action options. Posible values:
 *   - load:
 *   - view:
 *   - item_delete:
 *   - delete:
 * @param integer $pid
 *   The image ID.
 * @param object $node_light
 *   An object containing the following values:
 *   - uid: The user ID.
 *   - nodetype: The node type.
 *   - nid: The node ID.
 *   - ref: The node translation reference ID.
 *   - gid: The gallery ID
 *   - gref: The gallery translation reference ID.
 *   - pid: The image ID.
 *   - preview_size: The preview size. Important to use for the layout.
 *   - extra_styles:
 *
 * An example of usage of this hook is live available from:
 * @link http://www.drupal.org/project/gallery_assist_comments @endlink
 *
 */
function hook_gallery_assist_api($op, $pid, $node_light) {
  $output = array();
  switch ($op) {
    case 'load':
      // example code here...
      break;

    case 'view':
      // The returned code can be an string.
      $output = your_function('Code (string)');
      // An simple array().
      $output = array(
        '#value' => your_function('Code (string)'),
        '#weight' => 20,
      );
      // Or an array() with multiple keys.
      $output['field_name_1'] = array(
        '#value' => your_function('Code (string)'),
        '#weight' => 15,
        '#prefix' => '<div style="height:20px;background:maroon;"></div>',
        '#suffix' => '<div style="height:20px;background:maroon;"></div>'
      );
      // Other example with array().
      $output['field_name_2'] = array(
        '#value' => your_function('Code (string)'),
        '#weight' => 15,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#prefix' => '<div style="height:20px;background:maroon;"></div>',
        '#suffix' => '<div style="height:20px;background:maroon;"></div>'
      );
      return $output;
      break;

    case 'item_delete':
      // example code here...
      break;

    case 'delete':
      // example code here...
      break;
  }
}
