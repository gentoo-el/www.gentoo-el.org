<?php
// $Id: gallery_assist.admin.inc,v 1.41 2010/10/29 13:00:16 jcmc Exp $

/**
 * @file
 * Administration page from Gallery Assist.
 */

/**
 * Info data.
 */
function gallery_assist_info_data() {
  $info = array();
  // Version.
  $info['drupal_version'] = array(
    '#value' => '<div><strong>Drupal</strong> version: '. VERSION .'</div>',
    '#weight' => 0
  );
  $info['jq_version'] = array(
    '#value' => '<div>Current used <strong>JQuery</strong> version: <script>var version = $().jquery; document.write(version)</script></div>',
    '#weight' => 1
  );
  $names = array_values(gallery_assist_get_assignments());
  // Assignments.
  $assigs = '<div>GA <strong>Assignments</strong>:';
  $assigs .= theme('item_list', $names, NULL, 'ul');
  $assigs .= '</div>';

  $info['assignments'] = array(
    '#value' => $assigs,
    '#weight' => 2
  );

  // Assist modules.
  $result = db_query("SELECT * FROM {system} WHERE name LIKE '%gallery_assist%'");
  while ($r = db_fetch_object($result)) {
    $info_file = drupal_parse_info_file(drupal_get_path('module', $r->name) .'/'. $r->name .'.info');

    if (!empty($info_file['name'])) {
      $rows[] = array(
        array('data' => ga_bold($info_file['name'], $r->status), 'class' => 'ga-nowrap'),
        array('data' => $r->status == 1 ? 'enabled' : 'disabled'),
        array('data' => $info_file['version'], 'class' => 'ga-nowrap'),
        array('data' => $info_file['description'], 'style' => 'ga-fullwidth')
      );
    }
  }
  $plural = count($rows) > 1 ? 's' : '';
  $header = array(
    l('Module'. $plural, 'admin/build/modules'),
    'Status',
    'Version',
    'Description'
  );
  $info['modules'] = array(
    '#value' => theme('table', $header, $rows),
    '#weight' => 3,
  );
  $info['copy'] = array(
    '#value' => '<div id="ga-devel-by">'. gallery_assist_copyright() .'</div>',
    '#weight' => 100,
  );

  $output = drupal_render($info);

  return $output;
}

function ga_bold($text, $status) {
  $output = $status == 1 ? '<strong>'. $text .'</strong>' : $text;
  return $output;
}

/**
 * Settings form.
 *
 * @ingroup forms
 */
function gallery_assist_settings($form_state, $type = 'gallery_assist', $type_name = '') {
  global $base_path;

  drupal_add_css(drupal_get_path('module', 'gallery_assist') .'/css/gallery_assist.css', 'module', 'all', FALSE);

  $form = array();

  $assigned = variable_get('gallery_assist_'. $type, 0) == 1 ? ' ('. t('enabled') .')' : ' ('. t('disabled') .')';

  $fieldname[$type] = $type == 'gallery_assist' ? t('Gallery Assist') : t('Gallery Assist on @typename', array('@typename' => $type_name));
  if ($fieldname[$type] != 'gallery_assist') {
    $breadcrumb = drupal_get_breadcrumb();
    $breadcrumb[] = l($fieldname[$type], 'admin/settings/gallery_assist/'. $type);
    drupal_set_breadcrumb($breadcrumb);
  }

  $settings_type = variable_get('gallery_assist_'. $type .'_data', FALSE);

  if (variable_get('gallery_assist_use_imagecache', 0) == 1) {
    $settings_ic = variable_get('gallery_assist_'. $type .'_ic_data', FALSE);
  }

  $settings = is_array($settings_ic) ? array_merge($settings_type, $settings_ic) : $settings_type;

  $form['gallery_assist_'. $type .'_settings'][$type] = array(
    '#type' => 'fieldset',
    '#title' => $fieldname[$type],
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => $type == 'gallery_assist' ? 0 : 1,
    '#weight' => 0,
  );

  if (module_exists('imagecache') && variable_get('gallery_assist_use_imagecache', 0) == 1) {
    $form['gallery_assist_'. $type .'_settings'][$type] += _gallery_assist_get_ic_settings_form($type, $settings);
  }
  else {
    $form['gallery_assist_'. $type .'_settings'][$type] += _gallery_assist_sizes_settings_form($type, $settings);
  }

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_upload_settings'] = array();
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_upload_settings'] = gallery_assist_upload_settings($type, $settings);

  // Layout.
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Layout'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['gallery_assist_'. $type .'_items_per_row'] = array(
    '#type' => 'select',
    '#title' => t('Thumbnails per row'),
    '#options' => drupal_map_assoc(array(t('unlimited'), 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#default_value' => $settings['items_per_row'],
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['gallery_assist_'. $type .'_rows_per_page'] = array(
    '#title' => t('Rows per page'),
    '#default_value' => $settings['rows_per_page'],
    '#type' => 'select',
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['gallery_assist_'. $type .'_t_items_per_row'] = array(
    '#type' => 'select',
    '#title' => t('Thumbnails per row for the teaser'),
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#default_value' => $settings['t_items_per_row'],
    '#description' => t('Default is 0. Use this value to overwrite the teaser nummber of thumbnails pro row.'),
  );
$test = module_invoke('image', 'image_get_presets');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['thumbnails'] = array(
    '#type' => 'fieldset',
    '#title' => t('Thumbnails'),
    '#prefix' => '<div class="ga-clear-both">',
    '#suffix' => '</div>',
    '#class' => 'ga-clear-both',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['thumbnails']['gallery_assist_'. $type .'_item_margin'] = array(
    '#type' => 'select',
    '#title' => t('Items margin'),
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#default_value' => $settings['item_margin'],
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['thumbnails']['gallery_assist_'. $type .'_item_padding'] = array(
    '#type' => 'select',
    '#title' => t('Items padding'),
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#default_value' => $settings['item_padding'],
    '#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['thumbnails']['gallery_assist_'. $type .'_item_border'] = array(
    '#type' => 'select',
    '#title' => t('Items border'),
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)),
    '#default_value' => $settings['item_border'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['thumbnails']['gallery_assist_'. $type .'_item_border_color'] = array(
    '#type' => 'textfield',
    '#size' => '15',
    '#class' => 'farbtastic-picker',
    '#title' => t('Items border color'),
    '#default_value' => check_plain($settings['item_border_color']),
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links'] = array(
    '#type' => 'fieldset',
    '#title' => t('Titles and Help links'),
  );

  $desc['layout']['show_backlink'] = t('Enable this parameter to display a back link if the breadcrumb is disabled.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links']['gallery_assist_'. $type .'_show_backlink'] = array(
    '#type' => 'select',
    '#title' => t('Back link'),
    '#options' => array(0 => t('off'), 1 => t('on')),
    '#default_value' => $settings['show_backlink'],
    '#description' => $desc['layout']['show_backlink'],
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
  );

//  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links']['gallery_assist_'. $type .'_show_title'] = array(
//    '#type' => 'select',
//    '#title' => t('Display titles'),
//    '#options' => array(0 => t('off'), 1 => t('on')),
//    '#default_value' => $settings['show_title'] ? $settings['show_title'] : 0,
//    '#description' => $desc['layout']['show_title'],
//  );

  $desc['layout']['title_substr'] = t('Adjust the length of the titles to the width of the miniatures.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links']['gallery_assist_'. $type .'_title_substr'] = array(
    '#title' => t('Cut item title text for display'),
    '#default_value' => check_plain($settings['title_substr']),
    '#type' => 'textfield',
    '#size' => 10,
    '#description' => $desc['layout']['title_substr'],
    '#suffix' => '</div>',
  );

  $desc['layout']['show_toggle'] = t('Toggle to show or hide the images titles.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links']['gallery_assist_'. $type .'_show_toggle'] = array(
    '#type' => 'select',
    '#title' => t('Title links toggle'),
    '#options' => array(0 => t('off'), 1 => t('on')),
    '#default_value' => $settings['show_toggle'],
    '#description' => $desc['layout']['show_toggle'],
  );

  $desc['layout']['position_toggle'] = t('The link can be possisioned acord your goal bellow or above of the gallery  container.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['help_links']['gallery_assist_'. $type .'_position_toggle'] = array(
    '#type' => 'select',
    '#title' => t('Title links position'),
    '#options' => array(0 => t('Above of the gallery container'), 1 => t('Below of the gallery container')),
    '#default_value' => $settings['position_toggle'],
    '#description' => $desc['layout']['position_toggle'],
  );

  // Default values to the positioning and behavior from GA gallery container..
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['teaser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Teaser container'),
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
    '#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page container'),
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
    '#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['nodes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node settings defaults'),
    //'#prefix' => '<div style="clear:both;" class="clear-block">',
    //'#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['teaser']['gallery_assist_'. $type .'_ga_align'] = array(
    '#type' => 'select',
    '#title' => t('Teaser container align'),
    '#options' => array('none' => t('none'), 'left' => t('left'), 'center' => t('center'), 'right' => t('right')),
    '#default_value' => $settings['ga_align'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['teaser']['gallery_assist_'. $type .'_t_ga_float'] = array(
    '#type' => 'select',
    '#title' => t('Teaser container float'),
    '#options' => array('none' => t('none'), 'left' => t('left'), 'right' => t('right')),
    '#default_value' => $settings['t_ga_float'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['page']['gallery_assist_'. $type .'_gap_align'] = array(
    '#type' => 'select',
    '#title' => t('Page container align'),
    '#options' => array('none' => t('none'), 'left' => t('left'), 'center' => t('center'), 'right' => t('right')),
    '#default_value' => $settings['gap_align'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['page']['gallery_assist_'. $type .'_p_ga_float'] = array(
    '#type' => 'select',
    '#title' => t('Page container float'),
    '#options' => array('none' => t('none'), 'left' => t('left'), 'right' => t('right')),
    '#default_value' => $settings['p_ga_float'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['nodes'][0] = array();
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_layout']['nodes'][0] += _gallery_assist_settings_nodes($settings, $type);

  // Pager.
  // $desc['pager'][''] = t('');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pager'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    //'#description' => t(''),
  );

  $desc['pager']['type'] = t('The pager can be numeric or graphic. <br />If graphic enabled it shows mini thumnails. The Gallery Assist Pager is customizable through a template.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_symbol'] = array(
    '#title' => t('Type'),
    '#default_value' => $settings['pager_symbol'],
    '#type' => 'select',
    '#options' => array('0' => t('Numeric'), '1' => t('Icons')),
    '#description' => $desc['pager']['type'],
    //'#prefix' => '<div class="gallery-assist-settings-left-half">',
  );

  $options['pager']['layout'] = gallery_assist_pager_options();
  $desc['pager']['layout'] = count($options['pager']['layout']) > 1 ? t('Choose a layout.') : t('To obtain the list of avaiable skins create a folder ga_pager in your theme folder and copy there the wanted skins from your_contributed_modules/gallery_assist/ga_pager to.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_layout'] = array(
    '#title' => t('Layout'),
    '#default_value' => $settings['pager_layout'],
    '#type' => 'select',
    '#options' => $options['pager']['layout'],
    '#description' => $desc['pager']['layout'],
  );

  $desc['pager']['position'] = t('Weight of the gallery pager in the GA Image View relative to the image-container.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_position'] = array(
    '#title' => t('Position'),
    '#default_value' => $settings['pager_position'],
    '#type' => 'select',
    '#options' => array('top' => t('top'), 'bottom' => t('bottom'), 'both' => t('both')),
    '#description' => $desc['pager']['position'],
    ////'#suffix' => '</div>',
  );

  $desc['pager']['pager_align'] = t('Weight of the gallery pager in the GA Image View relative to the image-container.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_align'] = array(
    '#title' => t('Pager align'),
    '#default_value' => $settings['pager_align'],
    '#type' => 'select',
    '#options' => array('center' => t('center'), 'left' => t('left'), 'right' => t('right')),
    '#description' => $desc['pager']['position'],
    ////'#suffix' => '</div>',
  );

  $desc['pager']['format'] = t('Choose a format for the pager. <br /> - Full: Display first, previous, ellips, next and last links.<br /> - Minimized: Display previous and next links.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_format'] = array(
    '#title' => t('Format'),
    '#default_value' => $settings['pager_format'],
    '#type' => 'select',
    '#options' => array(0 => t('full'), 1 => t('minimized')),
    '#description' => $desc['pager']['format'],
    ////'#suffix' => '</div>',
  );

  $desc['pager']['visibles'] = t('Choose how many items should having the GA-pager on this node type.');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_visibles'] = array(
    '#type' => 'textfield',
    '#title' => t('Quantity of visible links'),
    '#default_value' => $settings['pager_visibles'],
    '#size' => 10,
    '#description' => $desc['pager']['visibles'],
  );

  $desc['pager']['pager_t_height'] = t('');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_t_height'] = array(
    '#title' => t('Thumbnail height'),
    '#default_value' => check_plain($settings['pager_t_height']),
    '#type' => 'textfield',
    '#size' => 10,
    '#description' => check_plain($desc['pager']['pager_t_height']),
    '#access' => is_numeric($settings['pager_presetid']) ? FALSE : TRUE,
  );

  $desc['pager']['pager_t_active_height'] = t('');
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_t_active_height'] = array(
    '#title' => t('Thumbnail height: active'),
    '#default_value' => check_plain($settings['pager_t_active_height']),
    '#type' => 'textfield',
    '#size' => 10,
    '#description' => check_plain($desc['pager']['pager_t_active_height']),
    '#access' => is_numeric($settings['pager_presetid']) ? FALSE : TRUE,
  );

  if (module_exists('imagecache')) {
    $presets = imagecache_presets();
    $options['pager']['thm_presets'] = array('none' => 'none');
    foreach ($presets as $presetid => $value) {
      $options['pager']['thm_presets'][$presetid] = $value['presetname'];
    }

    $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_pager']['gallery_assist_'. $type .'_pager_presetid'] = array(
      '#type' => 'select',
      '#title' => t('ImageCache preset'),
      '#default_value' => $settings['pager_presetid'],
      //'#options' => _gallery_assist_get_ic_presets(),
      '#options' => $options['pager']['thm_presets'],
      '#description' => t('Current used setting: @pager_thm_presetname', array('@pager_thm_presetname' => $options['pager']['thm_presets'][$settings['pager_presetid']])),
    );
  }

  // Others.
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others'] = array(
    '#type' => 'fieldset',
    '#title' => t('Others'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    //'#access' => FALSE,
  );

//  $h_li = array(
//    t('privat: <em>Only the owner can see the gallery.</em>'),
//    t('public: <em>Public for all authenticated users.</em>'),
//    t('public for all: <em>Public for all users and anonymous.</em>'),
//  );
//  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_public_status'] = array(
//    '#type' => 'select',
//    '#title' => t('Gallery Public Status'),
//    '#default_value' => $settings['public_status'],
//    '#options' => array(0 => t('privat'), 1 => t('public'), 2 => t('public for all')),
//    '#description' => theme('item_list', $h_li, NULL, 'ul'),
//  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_max_items'] = array(
    '#type' => 'textfield',
    '#title' => t('Images per node'),
    '#default_value' => check_plain($settings['max_items']),
    '#size' => 8,
    '#description' => t('Allowed amount of images per article (node)'),
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_max_form_items'] = array(
    '#type' => 'select',
    '#title' => t('Amount of uploads fields'),
    '#default_value' => $settings['max_form_items'],
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
    '#description' => t('Allowed amount of form upload fields per article (node)'),
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_graphlinks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use graphic links by editing of gallery images'),
    '#default_value' => $settings['graphlinks'],
    '#size' => 8
  );

  if (module_exists('pathauto')) {
    $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_build_aliases'] = array(
      '#type' => 'checkbox',
      '#title' => t('Build aliases.'),
      '#description' => t('The module pathauto is activated. Check to build aliases from gallery items.'),
      '#default_value' => $settings['build_aliases'],
      '#size' => 8
    );
  }

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_others']['gallery_assist_'. $type .'_hide_node_first_notice'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide GA head notice'),
    '#default_value' => variable_get("hide_node_first_notice_for_{$type}", FALSE),
    '#description' => t('Hide the notice "This node must be saved FIRST ..." that appears on the top of the creation of a node. <br />If module "<a href="@mnb" target="_blank">More node buttos</a>" is installed and you have enabled the "<a href="@screens" target="_blank">Save and continue</a>" button is this notice not more necessary.', array('@mnb' => url('http://www.drupal.org/project/more_node_buttons'), '@screens' => url('http://www.assist-series.com/content/use-module-more-node-buttons-ga'))),
    '#size' => 8
  );

  if (module_exists('gallery_assist_item')) {
    $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_item'] = array(
      '#type' => 'fieldset',
      '#title' => t('Display-Item Effects'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      //'#description' => t(''),
    );
  }

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Permission settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Allow gallery owners the configuration of some settings on their nodes with gallerie functionality.'),
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_common_settings_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('<strong>Gallery Assist (GA) settings::Common</strong>'),
    '#default_value' => $settings['common_settings_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_gallery_public_status_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA): settings::Common::Public status'),
    '#default_value' => $settings['gallery_public_status_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_container_format_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Display this gallery as'),
    '#default_value' => $settings['container_format_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_gallery_container_weight_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Gallery container weight'),
    '#default_value' => $settings['gallery_container_weight_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_show_in_homepage_block_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Show in block'),
    '#default_value' => $settings['show_in_homepage_block_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_show_in_userprofile_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Show in user profile'),
    '#default_value' => $settings['show_in_userprofile_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_show_titles_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Show titles'),
    '#default_value' => $settings['show_titles_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_gallery_items_shadow_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Common::Shadow'),
    '#default_value' => $settings['gallery_items_shadow_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_extra_layout_settings_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('<strong>Gallery Assist (GA) settings::Layout</strong>'),
    '#default_value' => $settings['extra_layout_settings_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_hide_teaser_container_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Extra-layout-Teaser::Hide gallery container'),
    '#default_value' => $settings['hide_teaser_container_allowed'],
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_users_perms']['gallery_assist_'. $type .'_display_download_link_allowed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gallery Assist (GA) settings::Extra-layout-Preview::Display download link'),
    '#default_value' => $settings['display_download_link_allowed'],
  );

  // Hidden values.
  $form['gallery_assist_'. $type .'_settings'][$type]['gallery_assist_'. $type .'_change'] = array(
    '#type' => 'hidden',
    '#value' => TRUE,
  );

  $form['gallery_assist_'. $type .'_settings'][$type]['node_type'] = array(
    '#type' => 'hidden',
    '#value' => $type,
  );

  $form['#submit'][] = 'gallery_assist_settings_submit';
  $form['#submit'][] = 'gallery_assist_4cviewer_nt_settings_submit';

  $form['submit']['save'] = array(
    '#type' => 'submit',
    '#name' => 'save',
    '#value' => t('Save'),
  );


  $form['gallery_assist_'. $type .'_settings']['info']  = array(
    '#type' => 'fieldset',
    '#title' => 'Settings Overview',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );
  $form['gallery_assist_'. $type .'_settings']['info']['content']  = gallery_assist_settings_info($type, $settings);
  $form['gallery_assist_'. $type .'_settings']['#suffix'] .= '<div id="ga-devel-by">'. gallery_assist_copyright() .'</div>';

  if (module_exists('i18n')) {
    $form['gallery_assist_'. $type .'_settings'][$type]['localize_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Behavior by translations'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 6
    );
    $form['gallery_assist_'. $type .'_settings'][$type]['localize_settings']['gallery_assist_'. $type .'_translation_behavior'] = array(
      '#type' => 'checkbox',
      '#title' => t('Take over the settings from the source'),
      '#default_value' => isset($settings['translation_behavior']) ? $settings['translation_behavior'] : TRUE
    );
    $form['gallery_assist_'. $type .'_settings'][$type]['localize_settings']['gallery_assist_'. $type .'_translation_behavior_allowed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow to change this parameter per node'),
      '#default_value' => isset($settings['translation_behavior_allowed']) ? $settings['translation_behavior_allowed'] : FALSE
    );
  }

  return $form;
}

function gallery_assist_settings_info($type, $settings) {
  $content = array();

  $content['links'] = array(
    '#type' => 'fieldset',
  );
  $content['links'] = array(
    '#type' => 'fieldset',
  );

  $names = array(
    'upload_thm' => t('Thumbnail upload size'),
    'upload_prev' => t('Preview upload sizev'),
    'items_per_row' => t('Items per row'),
    't_items_per_row' => t('Thumbnails per row for the teaser'),
    'rows_per_page' => t('Rows per page'),
    'thumbnail_size' => t('Thumbnail size'),
    'thumbnail_size_plus' => t('Thumbnail size plus'),
    'preview_size' => t('Preview size'),
    'item_margin' => t('Item margin'),
    'item_padding' => t('Item padding'),
    'show_in_homepage_block' => t('Show in block'),
    'item_border' => t('Item border'),
    'item_border_color' => t('Item border color'),
    'show_backlink' => t('Show backlink'),
    'show_toggle' => t('Show toggle'),
    'position_toggle' => t('Position toggle'),
    'title_substr' => t('Title substr'),
    'pager_symbol' => t('Pager symbol'),
    'pager_layout' => t('Pager layout'),
    'pager_position' => t('Pager position'),
    'pager_visibles' => t('Pager visibles'),
    'show_title' => t('Show title'),
    'show_layout' => t('Show layout'),
    'force_image_height' => t('Force image height'),
    'ga_align' => t('Gallery container align setting in the teaser view'),
    'gap_align' => t('Gallery container align setting in the page view'),
    't_ga_float' => t('Gallery container float setting in the teaser view'),
    'p_ga_float' => t('Gallery container float setting in the page view'),
    'shadow' => t('Shadow'),
    'hide_in_teaser' => t('Hide Gallery container in the teaser view'),
    'show_download_link' => t('Show download link'),
    'item_effect' => t('Item effect'),
    'max_items' => t('Maximal items per gallery'),
    'max_form_items' => t('Maximal Items in the form'),
    'validate_upload' => t('Validate upload'),
    'file_size' => t('Allowed maximale file size'),
    'user_size' => t('Allowed maximale memory size from the user'),
    'resolution' => t('Resolution restriction'),
    'extensions' => t('Allowed file extensions'),
    'public_status' => t('Public status'),
    'admin_public_status' => t('Admin public status'),
    'pager_t_height' => t('Pager thumbnail height'),
    'pager_t_active_height' => t('Pager thumbnail height active'),
    'extra_layout_settings_allowed' => t('Allow users to overwrite the layout settings'),
    'common_settings_allowed' => t('Allow users to overwrite the common settings'),
    'hide_teaser_container_allowed' => t('Allow users to overwrite the hide teaser setting'),
    'display_download_link_allowed' => t('Allow users to overwrite the display download link setting'),
    'container_format_allowed' => t('Allow users to overwrite the container format setting'),
    'gallery_container_weight_allowed' => t('Allow users to overwrite the gallery container weight setting'),
    'show_in_homepage_block_allowed' => t('Allow users to overwrite the show_in_homepage_block setting'),
    'gallery_public_status_allowed' => t('Allow users to overwrite the gallery_public_status setting'),
    'show_titles_allowed' => t('Allow users to overwrite the show_titles setting'),
    'show_in_userprofile_allowed' => t('Allow users to overwrite the show_in_userprofile setting'),
    'gallery_items_shadow_allowed' => t('Allow users to overwrite the gallery_items_shadow setting'),
    'ga_db_order_field' => t('DB-Order field'),
    'ga_db_order' => t('Order'),
    'use_imagecache' => t('Imagecache as preset builder'),
    'thm_presetname' => t('Thumbnail preset name'),
    'prev_presetname' => t('Preview preset name'),
    'thm_presetid' => t('Thumbnail preset id'),
    'prev_presetid' => t('Preview preset id'),
    'gallery_call_type' => 'Gallery call type',
    'ga_public_status' => t('Public status'),
    'pager_presetid' => t('Pager preset id'),
    'pager_presetname' => t('Pager preset name'),
    'pager_format' => t('Pager format'),
    'ga_item_title_type' => t('Item titles type'),
    'graphlinks' => t('Use graphic links'),
    'in_profile' => t('Show gallery in the user profile view'),
    'disable_margin_top' => t('Disable the margin-top css format'),
    'gallery_assist_weight' => t('Gallery container weight'),
  );

  foreach ($settings as $k => $v) {
//    if ($k == 'layout') {
//      foreach ($settings['layout'] as $lk => $lv) {
//        if (empty($names[$lk])) {
//          $names[$lk] = $k;
//        }
//        $content[$lk] = array(
//          '#value' => '<div><em>'. $names[$lk] .'</em>: <strong>'. $lv .'</strong></div>',
//        );
//      }
//    }
//    elseif ($k == 'validate_upload') {
//      foreach ($settings['validate_upload'] as $vk => $vv) {
//        if (empty($names[$vk])) {
//          $names[$vk] = $vk;
//        }
//        $content[$vk] = array(
//          '#value' => '<div><em>'. $names[$vk] .'</em>: <strong>'. $vv .'</strong></div>',
//        );
//      }
//    }
//    else {
      if (empty($names[$k])) {
        $names[$k] = $k;
      }
      $content[$k] = array(
        '#value' => '<div><em>'. $names[$k] .'</em>: <strong>'. $v .'</strong></div>',
      );
//    }
  }

  return $content;
}

/**
 * List all existing content types for the assignment of the gallery functionality.
 *
 * @ingroup forms
 */
function _gallery_assist_assignments_form() {
  $form = array();

  $types = node_get_types();
  unset($types['gallery_assist']);
  foreach ($types as $typ => $v) {
    $default_value = variable_get('gallery_assist_'. $v->type, 0);

    $form['gallery_assist_assignments']['gallery_assist_'. $v->type] = array(
      '#type' => 'checkbox',
      '#title' => $v->name,
      '#default_value' => $default_value,
    );

    $form['gallery_assist_assignments']['gallery_assist_'. $v->type .'_changed'] = array(
      '#type' => 'hidden',
      '#value' => $default_value,
    );
  }

  return $form;
}

/**
 * Build the tab with extra settings for Gallery Assist.
 * It is used to display some general settings and the forms from the submodules.
 *
 * @ingroup forms
 */
function gallery_assist_settings_extras() {
  global $user, $base_path;

  $import_path = file_directory_path() .'/u'. $user->uid .'/import';
  $dscan1 = file_scan_directory($import_path, '.', array(), 0, FALSE);
  $paths1 = array_keys($dscan1);

  $form = array();
  $form['gallery_assist_extras'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gallery Assist Extras'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  //
  $form['gallery_assist_extras']['gallery_assist_upload_conf'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload proccess'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    //'#description' => t('Better only for experts.'),
    '#weight' => -50,
  );
  $form['gallery_assist_extras']['gallery_assist_upload_conf']['gallery_assist_upload_method'] = array(
    '#type' => 'select',
    '#title' => t('Files save method'),
    '#default_value' => variable_get('gallery_assist_upload_method', 1),
    '#options' => array( 0 => t('Rename files'), 1 => t('Replace files')),
    '#description' => 'If you choose "Replace files" existing files will be replaced by uploading files through the new file with the same name.<br>If you choose "Rename files" new files with same name as existing files will be renamed with a counter at the end.',
  );

  // Assignments
  if (user_access('administer gallery_assist')) {
    $form['gallery_assist_extras']['gallery_assist_assignments'] = array(
      '#type' => 'fieldset',
      '#title' => t('Gallery functionality assignments'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => -50,
      '#description' => t('Assign the gallery functionality to other content types.'),
    );

    $form['gallery_assist_extras']['gallery_assist_assignments'] += _gallery_assist_assignments_form();
  }

  if (module_exists('imce')) {
    $form['gallery_assist_extras']['gallery_assist_import_directories'] = array(
      '#type' => 'radios',
      //'#title' => '',
      '#options' => $paths1,
    );
  }

  $form['gallery_assist_extras']['others'] = array(
    '#type' => 'fieldset',
    '#title' => t('Others'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 40,
  );

  $form['gallery_assist_extras']['others']['gallery_assist_editform_pager_limit'] = array(
    '#type' => 'select',
    '#title' => t('Pager limit in the node edit form'),
    '#description' => t('Limit of the number of listed images in the edit form.'),
    '#default_value' => variable_get('gallery_assist_editform_pager_limit', 'none'),
    '#options' => drupal_map_assoc(array('none', 10, 15, 20, 25, 30, 35, 40, 45, 50)),
  );

  $form['gallery_assist_extras']['others']['gallery_assist_forms_possition'] = array(
    '#type' => 'textfield',
    '#title' => t('Gallery Assist form fieldsets weight'),
    '#description' => t('A little help to set the weight of fieldsets.'),
    '#default_value' => variable_get('gallery_assist_forms_possition', -2),
    '#size' => 8
  );

  $form['gallery_assist_extras']['clear_cached_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clear cached data from Gallery Assist'),
    '#weight' => 50,
    '#description' => t('Warning: high-traffic sites with many galleries will experience performance slowdowns while cached data is rebuilt.'),

  );

  $form['gallery_assist_extras']['clear_cached_data']['gallery_assist_clearthecache'] = array(
    '#type' => 'submit',
    '#title' => t('Clear the Gallery Assist cached data'),
    '#name' => 'clearthecache',
    '#value' => t('Clear the Gallery Assist cached data'),
    '#submit' => array('gallery_assist_clearthecache'),
  );

  $form['submit']['save'] = array(
    '#type' => 'submit',
    '#name' => 'save',
    '#value' => t('Save'),
  );

  $form['gallery_assist_extras']['#suffix'] = '<div id="ga-devel-by">'. gallery_assist_copyright() .'</div>';
  $form['#submit'][] = 'gallery_assist_nodetypes_submit';
  $form['#validate'][] = 'gallery_assist_nodetypes_submit_validate';

  return $form;
}

/**
 * List the aviable Gallery Assist Pager layouts.
 *
 * @ingroup forms
 */
function gallery_assist_pager_options() {
  $pager_path_theme = drupal_get_path('theme', variable_get('theme_default', FALSE)) .'/ga_pager';
  $scan['theme'] = file_scan_directory($pager_path_theme, '.', $nomask = array('.', '..', 'README.txt'), 0, FALSE);
  $pager_options = array('default' => 'default');

  foreach ($scan as $dscan) {

    foreach ($dscan as $p => $d) {
      if (!is_file($d->basename)) {
        $pager_options[$d->basename] = $d->basename;
      }
    }

  }

  return $pager_options;
}

/**
 * The Gallery Assist default form to customize the upload sizes and the display sizes.
 * The two separated size-settings are necessary to obtain a little of flexibility by setup the  layout ( in cases where ImageCache is not installed).
 *
 * @ingroup forms
 */
function _gallery_assist_sizes_settings_form($type, $settings = array()) {

  $form = array();
  // Upload.
  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sizes'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload'),
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
    '#suffix' => '</div>',
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_upload']['gallery_assist_'. $type .'_upload_thm'] = array(
    '#type' => 'textfield',
    '#title' => t('Thumbnail upload size'),
    '#size' => 5,
    '#default_value' =>  $settings['upload_thm'],
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_upload']['gallery_assist_'. $type .'_upload_prev'] = array(
    '#type' => 'textfield',
    '#title' => t('Preview upload size'),
    '#size' => 5,
    '#default_value' => $settings['upload_prev'],
  );

  // Layout.
  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display'),
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
    '#suffix' => '</div>',
  );

  if ($settings['thumbnail_size'] == '' && $settings['thumbnail_size'] == 0) {
    $settings['thumbnail_size'] = $settings['thumbnail_size'] = 100;
  }

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes']['gallery_assist_'. $type .'_thumbnail_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Thumbnail display size'),
    '#size' => 5,
    '#default_value' => $settings['thumbnail_size'],
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes']['gallery_assist_'. $type .'_preview_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Preview display size'),
    '#size' => 5,
    '#default_value' => $settings['preview_size'],
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes']['gallery_assist_'. $type .'_thumbnail_size_plus'] = array(
    '#type' => 'textfield',
    '#title' => t('Extra thumbnail custom size'),
    '#size' => 5,
    '#default_value' => $settings['thumbnail_size_plus'],
    '#description' => t('Extend the width of thumbnails needed for layout.'),
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes']['gallery_assist_'. $type .'_force_image_height'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force image height'),
    '#size' => 5,
    '#default_value' => $settings['force_image_height'],
    '#access' => module_exists('imagecache') && variable_get('gallery_assist_use_imagecache', 0) == 1 ? TRUE : FALSE,
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_lsizes']['gallery_assist_'. $type .'_disable_margin_top'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable margin-top'),
    '#size' => 5,
    '#default_value' => $settings['disable_margin_top'],
    '#access' => module_exists('imagecache') && variable_get('gallery_assist_use_imagecache', 0) == 1 ? TRUE : FALSE,
  );

  return $form;
}

/**
 * This form replace the sizes form and shows the ImageCache presets.
 *
 * @ingroup forms
 */
function _gallery_assist_get_ic_settings_form($type, $settings) {
  $form = array();

  // Upload.
  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes'] = array(
    '#type' => 'fieldset',
    '#title' => t('ImagaCache presets'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $presets = imagecache_presets();
  foreach ($presets as $presetid => $value) {
    $options[$presetid] = $value['presetname'];
  }

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_thm_presetid'] = array(
    '#type' => 'select',
    '#title' => t('Thumbnail preset'),
    '#default_value' => $settings['thm_presetid'],
    '#options' => $options,
    '#description' => t('Current used setting: @thm_presetname', array('@thm_presetname' => $settings['thm_presetname'])),
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_prev_presetid'] = array(
    '#type' => 'select',
    '#title' => t('Preview preset'),
    '#default_value' => $settings['prev_presetid'],
    '#options' => $options,
    '#description' => t('Current used setting: @prev_presetname', array('@prev_presetname' => $settings['prev_presetname'])),
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_force_image_height'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force image height'),
    '#size' => 5,
    '#default_value' => $settings['force_image_height'],
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_disable_margin_top'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable margin-top'),
    '#size' => 5,
    '#default_value' => $settings['disable_margin_top'],
    '#access' => module_exists('imagecache') && variable_get('gallery_assist_use_imagecache', 0) == 1 ? TRUE : FALSE,
  );

  $form['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_sizes']['gallery_assist_'. $type .'_thumbnail_size_plus'] = array(
    '#type' => 'textfield',
    '#title' => t('Extra thumbnail custom size'),
    '#size' => 5,
    '#default_value' => $settings['thumbnail_size_plus'],
    '#description' => t('Extend the width of thumbnails needed for layout.'),
  );

  return $form;
}

/**
 * Menu callback for the upload settings form.
 * A takeover from upload module.
 */
function gallery_assist_upload_settings($type, $settings) {
  if (!empty($settings['resolution'])) {
    $sizes = explode('x', $settings['resolution']);
    $compare1 = $sizes[0] > $sizes[1] ? $sizes[0] : $sizes[1];
    $compare2 = module_exists('imagecache') && variable_get('gallery_assist_use_imagecache', 0) == 1 ? $settings['preview_size'] : $settings['upload_prev'];
    if ($compare1 < $compare2) {
      $settings['resolution'] = $compare2 .'x'. $compare2;
    }
  }

  $form['ga_upload_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node type upload settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#size' => -10,
  );

  $desc['upload']['resolution'] = t('The maximum allowed image size (e.g. 640x480). Set to 0 for no restriction. If an <a href="!image-toolkit-link">image toolkit</a> is installed, files exceeding this value will be scaled down to fit.', array('!image-toolkit-link' => url('admin/settings/image-toolkit')));
  $form['ga_upload_settings'][$type]['gallery_assist_'. $type .'_resolution'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum resolution for uploaded images'),
    '#default_value' => empty($settings['resolution']) ? 0 : $settings['resolution'],
    '#size' => 15,
    '#maxlength' => 10,
    '#description' => $desc['upload']['resolution'],
    '#field_suffix' => '<kbd>'. t('WIDTHxHEIGHT') .'</kbd>'
  );

  $desc['upload']['extensions'] = t('Extensions that users can upload. Separate extensions with a space and do not include the leading dot. Alloved are jpg jpeg gif png.');
  $form['ga_upload_settings'][$type]['gallery_assist_'. $type .'_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Default permitted file extensions'),
    '#default_value' => $settings['extensions'],
    '#maxlength' => 255,
    '#description' => $desc['upload']['extensions'],
  );

  $desc['upload']['file_size'] = t('The default maximum file size a user can upload. If an image is uploaded and a maximum resolution is set, the size will be checked after the file has been resized. Use dot and number (.5) for an file size smoll as 1MB.');
  $form['ga_upload_settings'][$type]['gallery_assist_'. $type .'_file_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Default maximum file size per upload'),
    '#default_value' => ($settings['file_size'] / 1024) / 1024,
    '#size' => 5,
    '#maxlength' => 5,
    '#description' => $desc['upload']['file_size'],
    '#field_suffix' => t('MB'),
  );

  $desc['upload']['user_size'] = t('The default maximum size of all files a user can have on the site.');
  $form['ga_upload_settings'][$type]['gallery_assist_'. $type .'_user_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Default total file size per user'),
    '#default_value' => ($settings['user_size'] / 1024) / 1024,
    '#size' => 5,
    '#maxlength' => 5,
    '#description' => $desc['upload']['user_size'],
    '#field_suffix' => t('MB'),
  );

  $form['ga_upload_settings']['upload_max_size'] = array('#value' => '<p>'. t('Your PHP settings limit the maximum file size per upload to %size.', array('%size' => format_size(file_upload_max_size()))) .'</p>');

  return $form;
}

function _gallery_assist_settings_nodes($settings, $type) {
  $form = array();

  $form['load_modi'] = array(
    '#type' => 'fieldset',
    '#title' => t('Items order'),
  );

  $form['load_modi']['gallery_assist_'. $type .'_ga_db_order_field'] = array(
    '#type' => 'select',
    '#title' => t('Field to order'),
    '#default_value' => $settings['ga_db_order_field'],
    '#options' => array(
      'a.weight' => t('Weight'), 
      'f.timestamp' => t('Upload date'), 
      'a.filename' => t('File name'), 
      'tp.ptitle' => t('Image title')
    ),
    '#prefix' => '<div class="gallery-assist-settings-left-half">',
    '#suffix' => '</div>',
  );

  $form['load_modi']['gallery_assist_'. $type .'_ga_db_order'] = array(
    '#type' => 'select',
    '#title' => t('Order type'),
    '#default_value' => $settings['ga_db_order'],
    '#options' => array(
      'DESC' => t('Descending'), 
      'ASC' => t('Ascending')
    ),
    //'#prefix' => '<div class="gallery-assist-settings-left-half">',
    //'#suffix' => '</div>',
  );

  $form['common'] = array(
    '#type' => 'fieldset',
    '#title' => t('Common'),
    //'#prefix' => '<div class="gallery-assist-settings">',
  );

  $form['common']['prefix'] = array(
    '#type' => 'markup',
    '#value' => '<div class="gallery-assist-settings-left-half">',
  );

  $h_li = array(
    t('privat: <em>Only the owner can see the gallery.</em>'),
    t('public: <em>Public for all authenticated users.</em>'),
    t('public for all: <em>Public for all users and anonymous.</em>'),
  );
  $form['common']['gallery_assist_'. $type .'_ga_public_status'] = array(
    '#type' => 'select',
    '#title' => t('Gallery Public Status'),
    '#default_value' => $settings['ga_public_status'],
    '#options' => array(0 => t('public'), 1 => t('privat') ,2 => t('public for all')),
    '#description' => theme('item_list', $h_li, NULL, 'ul'),
  );

  $form['common']['gallery_assist_'. $type .'_gallery_assist_weight'] = array(
    '#type' => 'select',
    '#title' => t('Gallery container weight'),
    '#default_value' => $settings['gallery_assist_weight'],
    '#options' => drupal_map_assoc(range(-50, 50)),
    '#description' => t('Choose the gallery container position on this node.'),
  );

  $form['common']['gallery_assist_'. $type .'_show_layout'] = array(
    '#type' => 'select',
    '#title' => t('Show this gallery as'),
    '#default_value' => empty($settings['show_layout']) ? 'grid' : $settings['show_layout'],
    '#options' => array('list' => t('List'), 'grid' => t('Grid')),
  );

  $form['common']['suffix0'] = array(
    '#type' => 'markup',
    '#value' => '</div><div>',
  );
  
  $this['show_title'] = $settings['show_title'] == 1 ? t('Uncheck to hide the images titles. With the creation of the article are images-titles shown.') : t('Check to display the image titles. With the creation of the article are images-titles shown.');
  $form['common']['gallery_assist_'. $type .'_show_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show titles'),
    '#default_value' => $settings['show_title'],
    '#description' => $this['show_title'],
  );

  $toptions = array(0 => t('Linked'), 1 => t('Text only'));
  if (module_exists('gallery_assist_lightboxes') && variable_get('gallery_assist_lightbox', FALSE) != 'none') {
    $toptions[2] = t('Linked to the lightbox');
  }
  $form['common']['gallery_assist_'. $type .'_ga_item_title_type'] = array(
    '#type' => 'select',
    '#title' => t('Titles format'),
    '#options' => $toptions,
    '#default_value' => $settings['ga_item_title_type'],
    '#access' => isset($settings['show_title']) ? TRUE : FALSE,
  );

  $form['common']['gallery_assist_'. $type .'_in_profile'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show in user profile'),
    '#default_value' => $settings['in_profile'],
    '#description' => '',
  );

  $this['help']['block'] = array(
    'module' => 'gallery_assist',
    'help_link' => 'gallery-assist-block-options',
    'text' => t('Check to display this Gallery in the Site Galleries Block.'),
  );
  $help_string = gallery_assist_advanced_help_builder($this['help']['block']);
  $form['common']['gallery_assist_'. $type .'_show_in_homepage_block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show in block'),
    '#default_value' => $settings['show_in_homepage_block'],
    '#description' => $help_string,
  );

  $form['common']['endfix'] = array(
    '#type' => 'markup',
    '#value' => '</div>',
  );

  $this['help']['layout'] = array(
    'module' => 'gallery_assist',
    'help_link' => 'gallery-assist-node-layout-settings',
    'text' => t(''),
  );
  $form['layout'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extra layout settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => gallery_assist_advanced_help_builder($this['help']['layout']),
  );

  $form['layout']['teaser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Teaser container'),
    '#prefix' => '<div class="gallery-assist-settings-left">',
    '#suffix' => '</div>',
    '#description' => t('Settings of the gallery container in the teaser view.'),
  );

  $form['layout']['page'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page container'),
    '#prefix' => '<div class="gallery-assist-settings-left">',
    '#suffix' => '</div>',
    '#description' => t('Settings of the gallery container in the page view.'),
  );

  $form['layout']['preview'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
    '#prefix' => '<div class="gallery-assist-settings-left">',
    '#suffix' => '</div>',
    '#description' => t('Settings of the gallery container in the image preview.'),
  );

  $this['help']['hide_teaser'] = array(
    'module' => 'gallery_assist',
    'help_link' => 'gallery-assist-node-layout-settings',
    'text' => t('If checked Gallery Assist will not show the gallery container in the teaser view.'),
  );
  $help_string = gallery_assist_advanced_help_builder($this['help']['hide_teaser']);
  $form['layout']['teaser']['gallery_assist_'. $type .'_hide_in_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide container in teaser'),
    '#default_value' => $settings['hide_in_teaser'],
    '#description' => $help_string,
  );

  $form['layout']['preview']['gallery_assist_'. $type .'_show_download_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display the download link'),
    '#default_value' => $settings['show_download_link'],
    '#prefix' => '<div class="gallery-assist-settings-left-30">',
    '#suffix' => '</div>',
  );

  $form['layout']['preview']['gallery_assist_'. $type .'_gallery_call_type'] = array(
    '#type' => 'select',
    '#title' => t('Gallery call type'),
    '#default_value' => $settings['gallery_call_type'],
    '#options' => array(0 => t('default'), 1 => t('single call'), 2 => t('single call random')),
    '#description' => t('Choose a option to jump directly to the first image (cover) or a random image without to view the page. Default is the normaly node call.'),
    //'#prefix' => '<div class="gallery-assist-settings-left-half">',
  );

  $form['layout']['preview']['gallery_assist_'. $type .'_show_download_link_old'] = array(
    '#type' => 'hidden',
    '#value' => $settings['show_download_link'],
  );

  return $form;
}
