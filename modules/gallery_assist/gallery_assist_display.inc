<?php
// $Id: gallery_assist_display.inc,v 1.45 2010/10/29 13:00:16 jcmc Exp $

/**
 * @file
 * Build and display the Gallery Assist Container.
 * This file will be included if the parameter "show_layout" is setted to "grid"
 * 
 * @see gallery_assist_include_display().
 */

/**
 * Display the Gallery Assist Container as grid.
 */
function gallery_assist_display($node, $opt='', $owner='') {
  global $user;

  $conf = $node->gallconf[$node->type];

  //$my_teaser_count = $conf['items_per_row'];
  $my_teaser_count = $conf['t_items_per_row'] > 0 ?  $conf['t_items_per_row'] : $conf['items_per_row'];
  $my_page_count = $conf['items_per_row'] == 'unlimited' ? count($node->gallitems) : $conf['items_per_row'] * $conf['rows_per_page'];
  if ($conf['items_per_row'] == 'unlimited' && $opt == 'teaser') {
    //$my_teaser_count = 3;
    $my_teaser_count = $conf['t_items_per_row'] > 0 ?  $conf['t_items_per_row'] : 3;
  }
  $my_previous = '‹ previous';
  $my_next = 'next ›';

  if (count($node->gallitems) == 0) { 
    return; 
  }

  foreach ($node->gallitems as $pid => $item) {
    $img_obj = image_get_info($item->tpath);
    if ($img_obj['width'] > $img_obj['height']) { 
      $img_obj['layout'] = 'h'; 
    }
    else { 
      $img_obj['layout'] = 'v'; 
    }

    foreach ($img_obj as $k => $v) {
      $node->gallitems[$pid]->$k = $v;
      $node->gallitems[$pid]->show_title = $node->show_title;
    }

    $node->gallitems[$pid]->my_nid = $node->nid;
    $my_item_box = gallery_assist_item_box($node->type, $node->gallitems[$pid], $conf, $img_obj);
    $my_boxes[] = $my_item_box;
    $block_boxes[] = $my_item_box;
  }

  // Prepare the pager OLD VARIANT OF gallery_assist.
  $node->gall_pager->show = $my_page_count;

  $node->gall_pager->cshow = $node->gall_pager->show;

  $theme = $user->theme ? $user->theme : variable_get('theme_default', FALSE);
  $my_pager_layout = $conf['pager_layout'];

  $my_grafic_next_path = file_exists(drupal_get_path('theme', $theme) .'/ga_pager/'. $my_pager_layout .'/next.png') ? drupal_get_path('theme', $theme) .'/ga_pager/'. $my_pager_layout .'/next.png' : drupal_get_path('module', 'gallery_assist') .'/ga_pager/default/next.png';
  $my_grafic_next = theme('image', $my_grafic_next_path, '', 'go to the next');
  $my_next = $conf['pager_symbol'] == 0 ? t('next ›') : $my_grafic_next;
  $my_grafic_previous_path = file_exists(drupal_get_path('theme', $theme) .'/ga_pager/'. $my_pager_layout .'/previous.png') ? drupal_get_path('theme', $theme) .'/ga_pager/'. $my_pager_layout .'/previous.png' : drupal_get_path('module', 'gallery_assist') .'/ga_pager/default/previous.png';
  $my_grafic_previous = theme('image', $my_grafic_previous_path, '', 'go to the previous');
  $my_previous = $conf['pager_symbol'] == 0 ? t('‹ previous') : $my_grafic_previous;

  $my_separator = $conf['pager_symbol'] == 0 ? ' | ' : '&nbsp;&nbsp;&nbsp;';

  if ($_GET['page'] == '' || $_GET['page'] == 1) {
    $pages = 0;
    $page = 2;
    $preview = '';
    if (count($my_boxes) <= $node->gall_pager->show) {
      $next = "";
    }
    else {

      $link_query['page'] = $page;
      if (isset($_GET['titles'])) {
        $link_query['titles'] = $_GET['titles'];
      }
      $next = l($my_next, $_GET['q'], array('html' => TRUE, 'attributes' => array('class' => $node->type .' pager-next'), 'query' => $link_query));
    }
  }
  else {
    $pages = $node->gall_pager->show * ($_GET['page'] - 1);
    $page = $_GET['page'];
    $node->gall_pager->cshow = $node->gall_pager->show * $_GET['page'];

    $link_query['page'] = ($_GET['page'] - 1);
    if (isset($_GET['titles'])) {
      $link_query['titles'] = $_GET['titles'];
    }
    $preview = l($my_previous, $_GET['q'], array('html' => TRUE, 'attributes' => array('class' => $node->type .' pager-previous'), 'query' => $link_query));
    if ($node->gall_pager->cshow < count($my_boxes)) {
      $link_query['page'] = ($_GET['page'] + 1);
      if (isset($_GET['titles'])) {
        $link_query['titles'] = $_GET['titles'];
      }
      $next = l($my_next, $_GET['q'], array('html' => TRUE, 'query' => $link_query));
    }
    else {
      $next = "";
    }
  }

  if (!empty($preview)) {
    $link[] = $preview;
  }
  if (!empty($next)) {
    $link[] = $next;
  }

  // Atrr.
  $c_w = $opt == 'teaser' && $node->gallconf[$node->type]['t_items_per_row'] > 0 ? $node->gallconf[$node->type]['t_container_width'] : $node->gallconf[$node->type]['container_width'];
  $my_attr_array = array(
    //'style' => 'max-width:'. $node->gallconf[$node->type]['container_width'] .'px;',
    'style' => 'max-width:'. $c_w .'px;',
  );
  $stl = array();
  $pstl = array();

  switch ($opt) {
    case 'teaser':
    for ($i = 0; $i < $my_teaser_count; $i++) {
      $boxes .= $my_boxes[$i];
    }
    $node->gallconf[$node->type]['op'] = 'teaser';

    if ($conf['ga_align'] != 'none' || $conf['t_ga_float'] != 'none') {

      //ALIGN TEASER
      switch ($conf['ga_align']) {
        case 'left':
          $stl['l'] = 'margin-right:auto';
          break;

        case 'center':
          $stl['ml'] = 'margin-left:auto';
          $stl['mr'] = 'margin-right:auto';
          break;

        case 'right':
          $stl['r'] = 'margin-left:auto';
          break;

      }

      //FLOAT TEASER
      switch ($conf['t_ga_float']) {
        case 'left':
          $stl['l'] = 'float:left';
          $stl['mr'] = 'margin-right:10px';
          unset($stl['ml']);
          break;

        case 'right':
          $stl['l'] = 'float:right';
          $stl['ml'] = 'margin-left:10px';
          unset($stl['mr']);
          break;

      }
    } // end (styles)
    break;

    case 'page': 
    for ($i=$pages; $i < $node->gall_pager->cshow; $i++) {
      if (drupal_strlen($my_boxes[$i]) > 0) {
        $count[] =  1;
      }
      $boxes .= $my_boxes[$i];
    }
    $node->gallconf[$node->type]['op'] = 'page';
    
    if ($conf['gap_align'] != 'none' || $conf['p_ga_float'] != 'none') { //(styles)

      //ALIGN PAGE
      $pstlx = $conf['pager_align'] == 'center' ? 'padding-right:10px;' : '';
      switch ($conf['gap_align']) {
        case 'left':
          $stl['l'] = 'margin-right:auto';
          $pstl['pl'] = $conf['pager_align'] == 'left' ? 'text-align:left;' : $pstlx .'text-align:'. $conf['pager_align'];
          break;

        case 'center':
          $stl['ml'] = 'margin-left:auto';
          $stl['mr'] = 'margin-right:auto';
          $pstlx = 'padding-right:10px;';
          $pstl['pl'] = $conf['pager_align'] == 'center' ? 'text-align:center;'. $pstlx : 'text-align:'. $conf['pager_align'];
          break;

        case 'right':
          $stl['r'] = 'margin-left:auto';
          $pstl['pl'] = $conf['pager_align'] == 'right' ? 'text-align:right;' : $pstlx. 'text-align:'. $conf['pager_align'];
          break;
      }

      //FLOAT PAGE
      switch ($conf['p_ga_float']) {
        case 'left':
          $stl['l'] = 'float:left';
          $stl['mr'] = 'margin-right:10px';
          unset($stl['ml']);
          break;

        case 'right':
          $stl['l'] = 'float:right';
          $stl['ml'] = 'margin-left:10px';
          unset($stl['mr']);
          break;
      }
    } // end (styles)
    break;
  }

  $my_attr_array['style'] .= implode(';', $stl); 

  //$extras['container_width'] = $conf['container_width'];
  $extras['container_width'] = $opt == 'teaser' ? $conf['t_container_width'] : $conf['container_width'];
  $extras['pager_position'] = $conf['pager_position'];
  $extras['pager_style'] = implode(';', $pstl);

  if (count($my_boxes) > $node->gall_pager->show && $opt == 'page') {
    $links = implode($my_separator, $link);
    $node_light = new stdClass();
    $pager .= theme('gallery_assist_pager', $node_light, $links, $extras);
  }

  $node->gallconf[$node->type]['my_attr'] = $node->gallconf[$node->type]['items_per_row'] == 'unlimited' ? '' : drupal_attributes($my_attr_array);
  $node->gallconf[$node->type]['nid'] = $node->nid;

  if ($owner == 'block') {
    $num = count($block_boxes) < $conf['block_gallery_visibles'] ? count($block_boxes) :  $conf['block_gallery_visibles'];

    $rand_boxes = array_rand($block_boxes, $num);

    if (count($rand_boxes) > 1) {
      foreach ($rand_boxes as $ks => $divs) {
        $bboxes .= $block_boxes[$ks];
        unset($block_boxes[$ks]);
      }
    }
    else {
      $bboxes .= $block_boxes[0];
    }
    return theme('gallery_assist_block_display', $bboxes, $node->gallconf[$node->type], $node->type);
  }
  else {
    return theme('gallery_assist_display', $pager, $boxes, $node->gallconf[$node->type], $node->type);
  }
}

/**
 * Build and return the gallery item box.
 */
function gallery_assist_item_box($type, $item, $conf, $img) {
  global $user;

  $shadow_style = '';

  if ($conf['shadow'] == 1) {
    $withshadow = ' shadow3';
    $shadow_style = '-moz-box-shadow:5px 5px 5px;';
    drupal_add_css(drupal_get_path('module', 'gallery_assist') .'/css/gallery_assist_shadow.css', 'module', 'all', FALSE);
  }

  $item->nodetype = $type;
  $my_title_height = $item->show_title == 1 && $conf['thumbnail_size'] >= 100 ? 30 : 0;
  $my_item_width = $conf['thumbnail_size'] + $conf['thumbnail_size_plus'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2);

  if (variable_get('gallery_assist_use_imagecache', 0) == 1 && $conf['force_image_height'] == 1) {
    $my_item_height = $conf['thumbnail_height_size'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2) + $my_title_height;
  }
  else {
    $my_item_height = $conf['thumbnail_size'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2) + $my_title_height;
  }

  $my_item_attr = array(
    'id' => 'item-box-'. $item->pid,
    'class' => $type .' item-box',
    'style' => 'height:'. $my_item_height .'px;'.
               'width:'. $my_item_width .'px;'. 
               $shadow_style
  );

  if ($conf['item_margin'] > 0) {
    $my_item_attr['style'] .= 'margin:'. $conf['item_margin'] .'px;';
  }
  $my_item_attr['style'] .= 'position:relative;';

  //$link = l('edit', 'node/gallery_assist/'. $item->nid .'/'. $item->pid, array('query' => array('page' => $_GET['page'])));
  $a_link = '<div id="image-box-'. $item->pid .'-links" class="'. $type .' image-box-links">[ '. $link .' ]</div>'."\n";

  $ga_item_attr  = drupal_attributes($my_item_attr);
  $edit_link  = $user->uid == $item->uid ? $a_link : '';
  $ga_item_box   = gallery_assist_image_box($type, $item, $conf, $img);
  $ga_item_title = $item->show_title == 1 && $conf['thumbnail_size'] >= 100 ? gallery_assist_title_box($item, $conf) : '';

  $output = theme('gallery_assist_item_box', $ga_item_box, $ga_item_title, $ga_edit_link, $ga_item_attr);

  return $output;
}

/**
 * Theme the Gallery Assist Item-Box.
 *
 * @ingroup themeable
 */
function theme_gallery_assist_item_box($ga_item_box, $ga_item_title, $ga_edit_link = '', $ga_item_attr = '') {
  $output  = '<div'. $ga_item_attr .'>'."\n";
  $output .= $ga_item_box;
  $output .= $ga_item_title;
  $output .= '</div>'."\n";

  return $output;
}

/**
 * Build and return the image box. 
 */
function gallery_assist_image_box($type, $item, $conf, $img) {
  global $user;
  if ($conf['shadow'] == 1) {
    $withshadow = ' shadow3';
    drupal_add_css(drupal_get_path('module', 'gallery_assist') .'/css/gallery_assist_shadow.css', 'module', 'all', FALSE);
  }

  if (variable_get('gallery_assist_use_imagecache', 0) == 1 && $conf['force_image_height'] == 1) {
    $my_image_height = $conf['thumbnail_height_size'];

    $my_box_attr = array(
      'id' => 'image-box-'. $item->pid,
      'class' => $type .' image-box'. $withshadow,
      'style' => 'height:'. $my_image_height .'px;'.
                 'width:'. $conf['thumbnail_size'] + ($conf['item_padding'] * 2) .'px;',
    );

    if ($conf['item_padding'] > 0) {
      $my_box_attr['style'] .= 'padding:'. $conf['item_padding'] .'px;';
    }

    if ($conf['item_border'] > 0) {
      $my_box_attr['style'] .= 'border:'. $conf['item_border'] .'px solid '. $conf['item_border_color'] .';';
    }

    $my_sheet_attr = array(
      'id' => 'image-sheet-'. $item->pid,
      'class' => $type .' image-sheet'. $my_img_behavior,
      'style' => 'height:'. $my_image_height .'px;'.
                 'width:'. $conf['thumbnail_size'] .'px;',
    );

    $img = gallery_assist_image_more_info($type, $img, $conf, $my_image_height);
  
    $my_layout = $img['pos'] == 'v' ? 'height' : 'width';
    $my_img_attr = array(
      $my_layout => $conf['thumbnail_size'],
      'style' => 'margin-top:'. $img['top'] .'px;',
    );
  }
  else {
    $my_box_attr = array(
      'id' => 'image-box-'. $item->pid,
      'class' => $type .' image-box'. $withshadow,
      'style' => 'height:'. $conf['thumbnail_size'] + ($conf['item_padding'] * 2) .'px;'.
                 'width:'. $conf['thumbnail_size'] + ($conf['item_padding'] * 2) .'px;',
    );

    if ($conf['item_padding'] > 0) {
      $my_box_attr['style'] .= 'padding:'. $conf['item_padding'] .'px;';
    }

    if ($conf['item_border'] > 0) {
      $my_box_attr['style'] .= 'border:'. $conf['item_border'] .'px solid '. $conf['item_border_color'] .';';
    }

    //$my_box_attr['style'] .= 'position:relative;';

    $my_sheet_attr = array(
      'id' => 'image-sheet-'. $item->pid,
      'class' => $type .' image-sheet'. $my_img_behavior,
      'style' => 'height:'. $conf['thumbnail_size'] .'px;'.
                 'width:'. $conf['thumbnail_size'] .'px;',
    );

    $img = gallery_assist_image_more_info($type, $img, $conf);

    $my_layout = $img['pos'] == 'v' ? 'height' : 'width';
    $my_img_attr = array(
      $my_layout => $conf['thumbnail_size'],
      'style' => 'margin-top:'. $img['top'] .'px;',
    );
  } // without

  $edit_link_attr = array(
    'class' => $type .' ga-item-edit-link',
    'style' => 'position:absolute;top:0px;left:0px;width:100%;',
  );

  $item->my_img_attr = $my_img_attr;
  $my_img = gallery_assist_prepare_thms($item, $conf);

  $get_values = $_GET;
  unset($get_values['q']);
  $edit_text = t('[edit]');
  $delete_text = t('[delete]');
  $elink = l($edit_text, 'node/ga_edit_one/'. $item->my_nid .'/'. $item->pid, array('query' => $get_values));
  $elink .= l($delete_text, 'node/ga_del_one/'. $item->my_nid .'/'. $item->pid, array('query' => $get_values));
//  $a_link = '<div id="image-box-'. $item->pid .'-links" class="'. $type .' image-box-links">[ '. $elink .' ]</div>'."\n";
  $node = new stdClass;
  $node->nid = $item->my_nid;
  $node->uid = $item->uid;
  $node->type = $type;
  $edit_link  = $user->uid == 1 || $user->uid == $item->uid || gallery_assist_check_access($node, 'edit') ? $elink : '';

  $link_query = array();
  if (isset($_GET['page'])) {
    $link_query['page'] = $_GET['page'];
  }
  if (isset($_GET['titles'])) {
    $link_query['titles'] = $_GET['titles'];
  }
  $output2  = '<div'. drupal_attributes($my_box_attr) .'>'."\n";
  $output2 .= '<div'. drupal_attributes($my_sheet_attr) .'>'."\n";
  $output2 .= '    '. l($my_img, 'node/'. $item->my_nid .'/'. $item->pid, array('html' => TRUE, 'query' => $link_query));
  $output2 .= '</div>'."\n";
  $output2 .= '</div>'."\n";
  $output2 .= '<div'. drupal_attributes($edit_link_attr) .'>'. $edit_link .'</div>'."\n";

  return $output2;
}

/**
 * Build and return the item title box. 
 */
function gallery_assist_title_box($item, $conf) {
  $points = drupal_strlen($item->ptitle) > $conf['title_substr'] ? "..." : '';
  $title = drupal_substr($item->ptitle, 0, $conf['title_substr']) . $points;
  $op = empty($conf['ga_item_title_type']) ? 0 : $conf['ga_item_title_type'];

  switch ($op) {
    case 0:
      $output  = '<div id="title-box-'. $item->pid .'" class="'. $item->nodetype .' title-box">'."\n";
      $output .= l($title, 'node/'. $item->my_nid .'/'. $item->pid, array('html' => TRUE, 'attributes' => array('title' => $item->ptitle)));
      $output .= '</div>'."\n";
      break;

    case 1:
      $output  = '<div id="title-box-'. $item->pid .'" class="'. $item->nodetype .' title-box">'."\n";
      $output .= $title;
      $output .= '</div>'."\n";
      break;
  }

  return $output;
}

/**
 * Theme the Gallery Assist Container as grid.
 *
 * @ingroup themeable
 */
function theme_gallery_assist_display($pager, $boxes, $conf, $type = 'gallery_assist') {
  $output  = '<div id="gallery-container-'. $conf['nid'] .'" class="'. $type .' gallery-container '. $conf['op'] .' clear-block"'. $conf['my_attr'] .'>';
  $output .= $conf['pager_position'] == 'top' || $conf['pager_position'] == 'both' ? $pager : '';
  $output .= $boxes;
  $output .= $conf['pager_position'] == 'bottom' || $conf['pager_position'] == 'both' ? $pager : '';
  $output .= '</div>';

  return  $output;
}
