// B44: complete two-arm matrix, build/b44-{axes,manifest}.json;
// build/b44-batch/results.json: 2/2 in3.53s, both exact952 bytes/37 blocks,
// all46 ordered targets, no sibling regression. Independent final checks
// prove all95 TU functions/allocated sections unchanged and full retail
// bytes/sites/targets for RVA0x1c47. The only axis is this unique statement:

// Baseline:
//     delete m_visibilityMap;

// Retained array-owner recovery (Open allocates new u16[MAP_WIDTH*MAP_HEIGHT]):
//     delete[] m_visibilityMap;

// The surrounding if(m_visibilityMap!=NULL), subsequent NULL assignment,
// and every other statement in advManager::Close remain unchanged.
