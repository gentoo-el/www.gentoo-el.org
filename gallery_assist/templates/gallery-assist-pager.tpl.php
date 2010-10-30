<?php
// $Id$

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
<style type="text/css">
/*
 * Hidde the following pager icons.
 * 
.item-list .pager li.pager-next, 
.item-list .pager li.pager-previous,
*/
.item-list .pager li.pager-ellipsis, 
.item-list .pager li.pager-last, 
.item-list .pager li.pager-first {
  display:none;
}

#gallery-container.teaser {
  border-right: 1px solid #000066;
  padding-right: 5px;
}
</style>
<div id="ga-pager-box" class="pager-<?php print $extras['pager_position']; ?> clear-block next-line" style="<?php print $extras['pager_style']; ?>">
  <?php print $links; ?>
</div>


