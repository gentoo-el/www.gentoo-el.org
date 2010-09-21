<?php
// $Id: gallery-assist-display.tpl.php,v 1.0 2009/11/19 16:11:12 jcmc Exp $

/**
 * @file
 * Default theme implementation to display the Gallery Assist Container.
 *
 * Available variables:
 * - $type: An string with the type of the node.
 * - $pager: An string with contains the rendered pager for the Gallery Assist page view.
 * - $boxes: An string with the rendered gallery item boxes.
 * - $conf: An array with the necesary content to display the gallery item.
 *   - contains
 *     - $conf->upload_thm
 *     - $conf->upload_prev
 *     - $conf->items_per_row
 *     - $conf->rows_per_page
 *     - $conf->thumbnail_size
 *     - $conf->thumbnail_size_plus
 *     - $conf->preview_size
 *     - $conf->item_margin
 *     - $conf->item_padding
 *     - $conf->item_border
 *     - $conf->item_border_color
 *     - $conf->show_backlink
 *     - $conf->show_toggle
 *     - $conf->show_title
 *     - $conf->layout
 *     - $conf->show_in_homepage_block
 *     - $conf->position_toggle
 *     - $conf->title_substr
 *     - $conf->pager_symbol
 *     - $conf->pager_layout
 *     - $conf->pager_position
 *     - $conf->pager_visibles
 *     - $conf->force_image_height
 *     - $conf->thumbnail_width_size
 *     - $conf->thumbnail_height_size
 *     - $conf->thm_presetid
 *     - $conf->prev_presetid
 *     - $conf->thm_presetname
 *     - $conf->prev_presetname
 *     - $conf->use_imagecache
 *     - $conf->show_layout
 *     - $conf->hide_in_teaser
 *     - $conf->spaces
 *     - $conf->container_width
 *     - $conf->op
 *     - $conf->my_attr
 * 
 * @see theme_gallery_assist_display()
 */
?>

<div id="gallery-container" class="<?php print $item->nodetype; ?> gallery-container <?php print $conf['op']; ?> clear-block" <?php print $conf['my_attr']; ?> >
<?php
  if ($conf['pager_position'] == 'top' || $conf['pager_position'] == 'both') {
    print $pager;
  }
  
  print $boxes;

  if ($conf['pager_position'] == 'bottom' || $conf['pager_position'] == 'both') {
    print $pager;
  }
  
  $conf['pager_position'] == 'top' || $conf['pager_position'] == 'both' ? $pager : '';
?>
</div>

