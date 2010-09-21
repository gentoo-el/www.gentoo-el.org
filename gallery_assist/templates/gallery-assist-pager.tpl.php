<?php
// $Id: gallery-assist-pager.tpl.php,v 1.0 2009/11/19 16:11:12 jcmc Exp $

/**
 * @file
 * Default theme implementation to display the Gallery Assist Items Pager.
 *
 * Available variables:
 * - $links: An string with contains the output from the theme_menu_list.
 *   - contains
 *     - $item->nid
 *     - $item->nodetype
 * - $extras: An array with the necesary content to display the gallery item.
 *   - contains
 *     - $extras['pager_position']
 *     - $extras['pager_style'] this variable is disabled (not in use at the moment).
 * 
 * @see gallery_assist_item_pager()
 */
?>

<div id="ga-pager-box" class="pager-<?php print $extras['pager_position']; ?> clear-block next-line" style="<?php print $extras['pager_style']; ?>">
  <?php print $links; ?>
</div>


