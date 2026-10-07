//! Opcode-level tests, built on hand-written streams.

use homm2_icon::{ColorOps, MaskOp, MaskOps, Op};

fn ops(stream: &[u8]) -> Vec<Op<'_>> {
    ColorOps::new(stream)
        .map(|op| op.expect("stream decodes"))
        .collect()
}

fn mask_ops(stream: &[u8]) -> Vec<MaskOp> {
    MaskOps::new(stream)
        .map(|op| op.expect("stream decodes"))
        .collect()
}

#[test]
fn worked_example_from_the_format_notes() {
    // 82 C3 2A 00 01 10 C0 CC 02 80
    let stream = [0x82, 0xc3, 0x2a, 0x00, 0x01, 0x10, 0xc0, 0xcc, 0x02, 0x80];
    let decoded = ops(&stream);

    assert_eq!(decoded[0], Op::Skip(2));
    assert_eq!(
        decoded[1],
        Op::Solid {
            len: 3,
            color: 0x2a
        }
    );
    assert_eq!(decoded[2], Op::EndOfRow);
    assert_eq!(decoded[3], Op::Literal(&[0x10]));
    let Op::Shadow { len, flags } = decoded[4] else {
        panic!("expected a shadow run");
    };
    assert_eq!(len, 2);
    assert_eq!(flags.level(), 3);
    assert!(flags.applies());
    assert!(flags.recolorable());
    assert_eq!(decoded.len(), 5);
}

#[test]
fn long_solid_escape_reads_a_count_byte() {
    assert_eq!(
        ops(&[0xc1, 200, 0x05, 0x80]),
        [Op::Solid { len: 200, color: 5 }]
    );
}

#[test]
fn short_solid_takes_its_length_from_the_opcode() {
    assert_eq!(
        ops(&[0xff, 0x09, 0x80]),
        [Op::Solid {
            len: 0x3f,
            color: 9
        }]
    );
}

#[test]
fn shadow_length_escapes_to_a_second_byte_when_the_short_field_is_zero() {
    let Op::Shadow { len, .. } = ops(&[0xc0, 0x44, 0xfe, 0x80])[0] else {
        panic!("expected a shadow run");
    };
    assert_eq!(len, 0xfe);
}

#[test]
fn colour_skips_are_six_bits_and_mask_skips_are_seven() {
    assert_eq!(ops(&[0xbf, 0x80]), [Op::Skip(0x3f)]);
    assert_eq!(mask_ops(&[0xff, 0x80]), [MaskOp::Skip(0x7f)]);
}

#[test]
fn both_dialects_stop_at_end_of_image() {
    assert_eq!(ops(&[0x80, 0x01, 0xaa]).len(), 0);
    assert_eq!(mask_ops(&[0x80, 0x01]).len(), 0);
}

#[test]
fn truncation_is_reported_rather_than_ignored() {
    // A literal claiming five payload bytes with only two present.
    let mut iter = ColorOps::new(&[0x05, 0x01, 0x02]);
    assert!(iter.next().is_some_and(|op| op.is_err()));
    assert!(iter.next().is_none(), "iterator fuses after an error");
}

#[test]
fn a_one_pixel_solid_run_is_unencodable_so_c1_stays_the_escape() {
    // 0xc1 is the long escape, never a one-pixel run.
    let Op::Solid { len, color } = ops(&[0xc1, 0x01, 0x42, 0x80])[0] else {
        panic!("expected a solid run");
    };
    assert_eq!((len, color), (1, 0x42));
}
