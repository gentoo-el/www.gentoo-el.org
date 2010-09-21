<?php
// $Id: gallery_assist_list_display.inc,v 1.x 2009/11/22 03:09:34 jcmc Exp $

/**
 * @file
 * Build and display the Gallery Assist Container.
 * This file will be included if the parameter "show_layout" is setted to "list"
 * 
 * @see gallery_assist_include_display().
 */

/**
 * Display the Gallery Assist Container as list.
 */
function gallery_assist_display_list($node, $opt='', $owner='') {
  global $user;

  $conf = $node->gallconf[$node->type];
  $conf['show_title'] = 0;

  $my_teaser_count = 1;

  $my_page_count = $conf['items_per_row'] == 'unlimited' ? count($node->gallitems) : $conf['items_per_row'] * $conf['rows_per_page'];
  if ($conf['items_per_row'] == 'unlimited' && $opt == 'teaser') {
    $my_teaser_count = 3;
  }
  $my_previous = '‹ previous';
  $my_next = 'next ›';

  if (count($node->gallitems) == 0) { 
    return; 
  }

  $e = 0;
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
      $node->gallitems[$pid]->show_title = 0;
    }
    
    $node->gallitems[$pid]->row = $i&1 ? 'odd' : 'even';

    $node->gallitems[$pid]->my_nid = $node->nid;
    $my_item_box = gallery_assist_item_box_list($node->type, $node->gallitems[$pid], $conf, $img_obj);
    $my_boxes[] = $my_item_box;
    ++$i;
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
      $next = l($my_next, $_GET['q'], array('html' => TRUE, 'attributes' => array('class' => $node->type .' pager-next'), 'query' => array('page' => $page, 'titles' => $_GET['titles'])));
    }
  }
  else {
    $pages = $node->gall_pager->show * ($_GET['page'] - 1);
    $page = $_GET['page'];
    $node->gall_pager->cshow = $node->gall_pager->show * $_GET['page'];

    $preview = l($my_previous, $_GET['q'], array('html' => TRUE, 'attributes' => array('class' => $node->type .' pager-previous'), 'query' => array('page' => ($_GET['page'] - 1), 'titles' => $_GET['titles'])));
    if ($node->gall_pager->cshow < count($my_boxes)) {
      $next = l($my_next, $_GET['q'], array('html' => TRUE, 'query' => array('page' => ($_GET['page'] + 1), 'titles' => $_GET['titles'])));
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

  $my_attr_array = array(
    'style' => 'max-width:'. $node->gallconf[$node->type]['container_width'] .'px;',
  );
  $stl = array();
  $pstl = array();

  switch ($opt) {
    case 'teaser':
    for ($i = 0; $i < $my_teaser_count; $i++) {
      $boxes .= $my_boxes[$i];
    }
    $node->gallconf[$node->type]['op'] = 'teaser';
  
    if ($conf['layout']['ga_align'] != 'none' || $conf['layout']['t_ga_float'] != 'none') {
      
      //ALIGN TEASER
      switch ($conf['layout']['ga_align']) {
        case 'center':
        case 'left':
          $stl['l'] = 'margin-right:auto';
          break;

        case 'right':
          $stl['r'] = 'margin-left:auto';
          break;

      }

      //FLOAT TEASER
      switch ($conf['layout']['t_ga_float']) {
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
    
    if ($conf['layout']['gap_align'] != 'none' || $conf['layout']['p_ga_float'] != 'none') { //(styles)

      //ALIGN PAGE
      switch ($conf['layout']['gap_align']) {
        case 'center':
        case 'left':
          $stl['l'] = 'margin-right:auto';
          $pstl['pl'] = 'text-align:left;';
          break;

        case 'right':
          $stl['r'] = 'margin-left:auto';
          $pstl['pl'] = 'text-align:right;';
          break;
      }

      //FLOAT PAGE
      switch ($conf['layout']['p_ga_float']) {
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

  $extras['container_width'] = $conf['container_width'];
  $extras['pager_position'] = $conf['pager_position'];
  $extras['pager_style'] = implode(';', $pstl);

  if (count($my_boxes) > $node->gall_pager->show && $opt == 'page') {
    $links = implode($my_separator, $link);
    $pager .= theme('gallery_assist_pager', $links, $extras);
  }

  $node->gallconf[$node->type]['my_attr'] = $node->gallconf[$node->type]['items_per_row'] == 'unlimited' ? '' : drupal_attributes($my_attr_array);
  $node->gallconf[$node->type]['nid'] = $node->nid;

  if ($owner == 'block') {
    $num = count($my_boxes) < $conf['block_gallery_visibles'] ? count($my_boxes) :  $conf['block_gallery_visibles'];

    $rand_boxes = array_rand($my_boxes, $num);
    
    foreach ($rand_boxes as $ks => $divs) {
      $b_boxes .= $my_boxes[$ks];
    }
    
    $b_invlisten = count($my_boxes) ? implode($my_boxes) : '';
    return theme('gallery_assist_block_display', $b_boxes . $b_invlisten, $node->gallconf[$node->type], $node->type);
  }
  else {
    return theme('gallery_assist_display_list', $pager, $boxes, $node->gallconf[$node->type], $node->type);
  }
}

/**
 * Build and return the gallery item box.
 */
function gallery_assist_item_box_list($type, $item, $conf, $img) {
  global $user;

  $item->nodetype = $type;
  $my_title_height = 0;
  $my_item_width = $conf['thumbnail_size'] + $conf['thumbnail_size_plus'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2);

  if (variable_get('gallery_assist_use_imagecache', 0) == 1 && $conf['force_image_height'] == 1) {
    $my_item_height = $conf['thumbnail_height_size'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2) + $my_title_height;
  }
  else {
    $my_item_height = $conf['thumbnail_size'] + ($conf['item_padding'] * 2) + ($conf['item_border'] * 2) + $my_title_height;
  }

  $my_item_list_box = array(
    'id' => 'item-box-row-'. $item->pid,
    'class' => $type .' item-box-row '. $item->row,
    'style' => 'height:'. ($my_item_height + 10) .'px;display:block;clear:left;',
  );

  $my_item_attr = array(
    'id' => 'item-box-'. $item->pid,
    'class' => $type .' item-box',
    'style' => 'width:'. $my_item_width .'px;',
  );
  $my_item_attr['style'] .= 'position:relative;';

  if ($conf['item_margin'] > 0) {
    $top_margin = $conf['item_margin'] / 2;
  }  
    $right_margin = $conf['item_margin'] > 10 ? $conf['item_margin'] : 10;
    $my_item_attr['style'] .= 'margin-right:'. $right_margin .'px;margin-top:'. $top_margin .'px;';

  //$link = l('edit', 'node/gallery_assist/'. $item->nid .'/'. $item->pid);
  //$a_link = '<div id="image-box-'. $item->pid .'-links" class="'. $type .' image-box-links">[ '. $link .' ]</div>';
  //$link = l('edit', 'node/gallery_assist/'. $item->nid .'/'. $item->pid);
  //$a_link = '<div id="image-box-'. $item->pid .'-links" class="'. $type .' image-box-links">[ '. $link .' ]</div>'."\n";

  $ga_item_attr  = drupal_attributes($my_item_attr);
  $ga_item_list_attr  = drupal_attributes($my_item_list_box);
  //$edit_link  = $user->uid == $item->uid ? $a_link : '';
  //$edit_link  = $user->uid == $item->uid ? $a_link : '';
  $ga_item_box   = gallery_assist_image_box_list($type, $item, $conf, $img);
  $ga_item_desc = '<div class="list-desc-container" style="padding-left:'. ($my_item_width + $right_margin) .'px;">'. $item->pdescription .'</div>';
  //$ga_edit_link  = $_GET['links'] = 'off' ? $edit_link : '';
  $ga_item_copyright = _filter_html(_filter_url($item->copyright, 1), 1);

  $output = theme('gallery_assist_item_box_list', $ga_item_list_attr, $ga_item_box, $item->ptitle, $ga_item_copyright, $ga_item_desc, $ga_edit_link, $ga_item_attr);

  return $output;
}

/**
 * Theme the Gallery Assist Item-Box.
 *
 * @ingroup themeable
 */
function theme_gallery_assist_item_box_list($ga_item_list_attr, $ga_item_box, $ga_item_title, $ga_item_copyright, $ga_item_desc, $ga_edit_link = '', $ga_item_attr) {
  $output  = '      <!-- Gallery Assist Item -->'."\n";
  $output .= '      <div'. $ga_item_list_attr .'>'."\n";
  $output .= '        <div'. $ga_item_attr .'>'."\n";
  $output .= $ga_edit_link;
  $output .= $ga_item_box;
  $output .= '        </div>'."\n";
  $output .= '        <div class="title-box-on-list">'. $ga_item_title .'</div>'."\n";
  $output .= !empty($ga_item_copyright) ? '        <div id="gallery-item-copyright" class="gallery-item-copyright">&copy; '. $ga_item_copyright .'</div>'."\n" : '';
  $output .= $ga_item_desc ."\n";
  $output .= '      </div>'."\n";

  return $output;
}

/**
 * Build and return the image box. 
 */
function gallery_assist_image_box_list($type, $item, $conf, $img) {
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
  
    $my_sheet_attr = array(
      'id' => 'image-sheet-'. $item->pid,
      'class' => $type .' image-sheet'. $my_img_behavior,
      'style' => 'height:'. $conf['thumbnail_size'] .'px;'.
                 'width:'. $conf['thumbnail_size'] .'px;',
    );

    $edit_link_attr = array(
      'class' => $type .' ga-item-edit-link',
      'style' => 'position:absolute;top:0px;left:0px;width:100%;',
    );

    $img = gallery_assist_image_more_info($type, $img, $conf);
  
    $my_layout = $img['pos'] == 'v' ? 'height' : 'width';
    $my_img_attr = array(
      $my_layout => $conf['thumbnail_size'],
      'style' => 'margin-top:'. $img['top'] .'px;',
    );
  } // Without force.

  $item->my_img_attr = $my_img_attr;
  $my_img = gallery_assist_prepare_thms($item, $conf);

  $get_values = $_GET;
  unset($get_values['q']);
  $link = l('[edit]', 'node/ga_edit_one/'. $item->my_nid .'/'. $item->pid, array('query' => $get_values));
  $a_link = '<div id="image-box-'. $item->pid .'-links" class="'. $type .' image-box-links">[ '. $link .' ]</div>'."\n";
  $edit_link  = $user->uid == 1 || $user->uid == $item->uid || user_access('edit gallery_assist content', $user) || user_access('edit gallery_assist_'. $type .' content', $user) ? $link : '';

  $output  = '<div'. drupal_attributes($my_box_attr) .'>'."\n";
  $output .= '  <div'. drupal_attributes($my_sheet_attr) .'>'."\n";
  $output .= '    '. l($my_img, 'node/'. $item->my_nid .'/'. $item->pid, array('html' => TRUE));
  $output .= '  </div>'."\n";
  $output .= '</div>'."\n";

  $output2  = '<div'. drupal_attributes($my_box_attr) .'>'."\n";
  $output2 .= '<div'. drupal_attributes($my_sheet_attr) .'>'."\n";
  $output2 .= '    '. l($my_img, 'node/'. $item->my_nid .'/'. $item->pid, array('html' => TRUE));
  $output2 .= '</div>'."\n";
  $output2 .= '</div>'."\n";
  $output2 .= '<div'. drupal_attributes($edit_link_attr) .'>'. $edit_link .'</div>'."\n";

  return $output2;
}

/**
 * Build and return the item title box. 
 */
function gallery_assist_title_box_list($item, $conf) {
  $points = drupal_strlen($item->ptitle) > $conf['title_substr'] ? "..." : '';
  $title = drupal_substr($item->ptitle, 0, $conf['title_substr']) . $points;

  $output  = '<div id="title-box-'. $item->pid .'" class="'. $item->nodetype .' title-box-on-list">'."\n";
  $output .= l($title, 'node/'. $item->my_nid .'/'. $item->pid, array('html' => TRUE, 'attributes' => array('title' => $item->ptitle)));
  $output .= '</div>'."\n";

  return $output;
}

/**
 * Theme the Gallery Assist Container as list.
 *
 * @ingroup themeable
 */
function theme_gallery_assist_display_list($pager, $boxes, $conf, $type = 'gallery_assist') {
  
  $output  = '<!-- Gallery Assist container -->'."\n";
  $output .= '    <div id="gallery-container-'. $conf['nid'] .'" class="'. $type .' gallery-container '. $conf['op'] .'  container-justify-'. $conf['container_justify'] .' container-justify-'. $conf['container_justify'] .' clear-block"'. $conf['$my_attr'] .'>'."\n";
  $output .= $conf['pager_position'] == 'top' || $conf['pager_position'] == 'both' ? $pager : '';
  $output .= $boxes;
  $output .= $conf['pager_position'] == 'bottom' || $conf['pager_position'] == 'both' ? $pager : '';
  $output .= '    </div>'."\n";
  $output .= '    <!-- End of Gallery Assist container -->'."\n";

  return  $output;
}

