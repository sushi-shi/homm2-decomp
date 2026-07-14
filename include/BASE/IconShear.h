#ifndef HOMM2_BASE_ICONSHEAR_H
#define HOMM2_BASE_ICONSHEAR_H

// Serialized sentinel used by icon decoders with a per-row horizontal shear table.
typedef enum IconShearConstant {
    ICON_SHEAR_SKIP_ROW = 0x7f
} IconShearConstant;

#endif // HOMM2_BASE_ICONSHEAR_H
