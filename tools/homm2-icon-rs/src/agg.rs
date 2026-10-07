//! Reader for the `.AGG` archive that ships the game's icons.
//!
//! ```text
//! +--------+----------------------+-----------------+------------------+
//! | u16    | count x 12-byte      | payload         | count x 15-byte  |
//! | count  | directory records    |                 | names, NUL padded|
//! +--------+----------------------+-----------------+------------------+
//!            id u32                                    at end of file
//!            offset u32
//!            size u32
//! ```
//!
//! The directory keys records by a hash of the name; the trailing name table is
//! what makes lookup by filename possible.
//!
//! Allocation-free: names are borrowed out of the trailing table and directory
//! records are decoded on demand, so opening an archive to pull one member does
//! not build a directory of eight hundred owned strings.

use crate::error::{Error, Result};

/// Bytes per directory record.
const RECORD_LEN: usize = 12;

/// Bytes per trailing name-table entry.
const NAME_LEN: usize = 15;

/// One archive member.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Entry<'a> {
    /// Member name with its NUL padding trimmed, borrowed from the archive.
    ///
    /// Kept as bytes because the field is a fixed-width DOS filename, not
    /// guaranteed UTF-8. Use [`Entry::name_str`] when you need text.
    pub name: &'a [u8],
    /// Name hash used by the game's own lookup.
    pub id: u32,
    /// Byte offset of the payload within the archive.
    pub offset: u32,
    /// Payload length in bytes.
    pub size: u32,
}

impl<'a> Entry<'a> {
    /// The name as text, when it is valid UTF-8.
    #[must_use]
    pub const fn name_str(&self) -> Option<&'a str> {
        match core::str::from_utf8(self.name) {
            Ok(text) => Some(text),
            Err(_) => None,
        }
    }

    /// Whether the name matches `other`, ignoring ASCII case.
    #[must_use]
    pub fn matches(&self, other: &str) -> bool {
        self.name.eq_ignore_ascii_case(other.as_bytes())
    }

    /// Whether the name ends with `suffix`, ignoring ASCII case.
    #[must_use]
    pub fn has_extension(&self, suffix: &str) -> bool {
        self.name
            .len()
            .checked_sub(suffix.len())
            .is_some_and(|at| self.name[at..].eq_ignore_ascii_case(suffix.as_bytes()))
    }
}

/// A parsed `.AGG` archive borrowing the whole file.
#[derive(Clone, Copy, Debug)]
pub struct Archive<'a> {
    data: &'a [u8],
    directory: &'a [u8],
    names: &'a [u8],
    count: usize,
}

impl<'a> Archive<'a> {
    /// Validates the directory and name table of an archive.
    ///
    /// # Errors
    ///
    /// Returns [`Error::Truncated`] when the directory or the trailing name
    /// table does not fit inside `data`.
    pub fn parse(data: &'a [u8]) -> Result<Self> {
        let count_bytes = data.first_chunk::<2>().ok_or(Error::Truncated {
            what: "archive header",
            needed: 2,
            available: data.len(),
        })?;
        let count = usize::from(u16::from_le_bytes(*count_bytes));

        let directory_end = 2 + count * RECORD_LEN;
        let directory = data.get(2..directory_end).ok_or(Error::Truncated {
            what: "archive directory",
            needed: directory_end,
            available: data.len(),
        })?;

        let names_len = count * NAME_LEN;
        let names_start = data.len().checked_sub(names_len).ok_or(Error::Truncated {
            what: "archive name table",
            needed: names_len,
            available: data.len(),
        })?;

        Ok(Self {
            data,
            directory,
            names: &data[names_start..],
            count,
        })
    }

    /// Number of members.
    #[must_use]
    pub const fn len(&self) -> usize {
        self.count
    }

    /// Whether the archive holds no members.
    #[must_use]
    pub const fn is_empty(&self) -> bool {
        self.count == 0
    }

    /// Decodes one directory record.
    #[must_use]
    pub fn entry(&self, index: usize) -> Option<Entry<'a>> {
        let record = self
            .directory
            .get(index.checked_mul(RECORD_LEN)?..)
            .and_then(<[u8]>::first_chunk::<RECORD_LEN>)?;
        let name = self
            .names
            .get(index.checked_mul(NAME_LEN)?..)
            .and_then(<[u8]>::first_chunk::<NAME_LEN>)?;
        let word = |at: usize| {
            u32::from_le_bytes([record[at], record[at + 1], record[at + 2], record[at + 3]])
        };
        let end = name.iter().position(|&byte| byte == 0).unwrap_or(NAME_LEN);
        Some(Entry {
            name: &name[..end],
            id: word(0),
            offset: word(4),
            size: word(8),
        })
    }

    /// Every member, in directory order.
    ///
    /// The iterator borrows the archive's bytes, not the `Archive` value, so it
    /// outlives the `&self` it came from. `Archive` is [`Copy`], which is what
    /// makes that possible without an allocation.
    #[must_use = "iterators are lazy"]
    pub fn entries(&self) -> impl Iterator<Item = Entry<'a>> + use<'a> {
        let archive = *self;
        (0..archive.count).filter_map(move |index| archive.entry(index))
    }

    /// Every member whose name ends in `.ICN`.
    #[must_use = "iterators are lazy"]
    pub fn icons(&self) -> impl Iterator<Item = Entry<'a>> + use<'a> {
        self.entries().filter(|entry| entry.has_extension(".ICN"))
    }

    /// The payload of `entry`, or `None` when its extent falls outside the file.
    #[must_use]
    pub fn payload(&self, entry: &Entry<'a>) -> Option<&'a [u8]> {
        let start = usize::try_from(entry.offset).ok()?;
        let len = usize::try_from(entry.size).ok()?;
        self.data.get(start..start.checked_add(len)?)
    }

    /// Looks a member up by name, case-insensitively.
    #[must_use]
    pub fn find(&self, name: &str) -> Option<Entry<'a>> {
        self.entries().find(|entry| entry.matches(name))
    }

    /// The payload of the member called `name`.
    #[must_use]
    pub fn get(&self, name: &str) -> Option<&'a [u8]> {
        self.payload(&self.find(name)?)
    }
}
