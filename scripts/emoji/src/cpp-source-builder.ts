import { join } from 'path';
import { mkdir, writeFile } from 'fs/promises';
import { spawnSync } from 'child_process';

type EmojiInfo = {
	emoji: string;
	name: string;
	group: number;
	keywords: number[];
	skinToneSupport: boolean;
};

const quoted = (s: string) => `"${s}"`;

export class CppSourceBuilder {
	private m_emojis: EmojiInfo[] = [];
	private m_keywords: string[] = [];
	private m_groups: string[] = [];
	private m_keywordMap = new Map<string, number>();
	private m_groupMap = new Map<string, number>();

	private buildItemStruct(info: EmojiInfo): string {
		return `\tEmojiData{ .emoji = "${info.emoji}", .name = ${quoted(info.name)}, .group = GRP(${info.group}), .keywords = {${info.keywords.map(idx => `KW(${idx})`).join(',')}}, .skinToneSupport = ${info.skinToneSupport}}`;
	}

	private buildHeader = (): string => {
const HEADER_BASE = `#pragma once
#include <vector>
#include <string_view>
#include <array>
#include <unordered_map>

struct EmojiData {
	std::string_view emoji;
	std::string_view name;
	std::string_view group;
	std::vector<std::string_view> keywords;
	bool skinToneSupport = false;
};

class StaticEmojiDatabase {
	public:
		StaticEmojiDatabase() = delete;
		static const std::array<EmojiData, ${this.m_emojis.length}>& orderedList();
		static const std::unordered_map<std::string, const EmojiData*>& mapping();
};
`

		return HEADER_BASE;
	}

	private buildStaticArray() {
		return `const std::array<EmojiData, ${this.m_emojis.length}> EMOJI_LIST = {\n${this.m_emojis.map(this.buildItemStruct).join(',\n')}\n};`;
	}
	
	private buildSource() {
		return `// clang-format off\n\n#include "emoji.hpp"\n#include <string_view>\n#include <array>\n\n${this.buildCategories()}\n\n${this.buildKeywords()}\n\n${this.buildStaticArray()}\n\nconst std::array<EmojiData, ${this.m_emojis.length}>& StaticEmojiDatabase::orderedList() { return EMOJI_LIST; }\n${this.buildMap()}`;
	}

	private buildCategories() {
		return `#define GRP(idx) GROUPS[idx]\n\nconstexpr std::array<std::string_view, ${this.m_groups.length}> GROUPS = {\n${this.m_groups.map(quoted).join(',')}\n};`;
	}

	private buildKeywords() {
		return `#define KW(idx) KEYWORDS[idx]\n\nconstexpr std::array<std::string_view, ${this.m_keywords.length}> KEYWORDS = {\n${this.m_keywords.map(quoted).join(',')}\n};`;
	}

	private buildMap() {
		return `const std::unordered_map<std::string_view, const EmojiData*> MAPPING = {
${this.m_emojis.map(({ emoji }, idx) => `{ ${quoted(emoji)}, &EMOJI_LIST[${idx}] }`).join(',\n')}
};
`
	}

	constructor() {
	}

	addEmoji(emoji: string, name: string, group: string, keywords: string[], skinToneSupport: boolean) {
		let groupIdx = -1;
		const keywordIndexes: number[] = [];
		
		if (this.m_groupMap.has(group)) {
			groupIdx = this.m_groupMap.get(group)!;
		} else {
			this.m_groupMap.set(group, this.m_groups.length);
			groupIdx = this.m_groups.length;
			this.m_groups.push(group);
		}

		for (const keyword of keywords) {
			if (keyword.includes("\\")) continue ;

			if (!this.m_keywordMap.has(keyword)) {
				this.m_keywordMap.set(keyword, this.m_keywords.length);
				keywordIndexes.push(this.m_keywords.length);
				this.m_keywords.push(keyword);
			} else {
				keywordIndexes.push(this.m_keywordMap.get(keyword)!);
			}
		}

		this.m_emojis.push({ emoji, name, group: groupIdx, keywords: keywordIndexes, skinToneSupport });
	}

	async build(outDir: string) {
		const sourceDst = join(outDir, "emoji.cpp");
		const  headerDst = join(outDir, "emoji.hpp");

		console.log(`Building source files for ${this.m_emojis.length} emojis...`);
		await mkdir(outDir, { recursive: true });

		await writeFile(join(outDir, "emoji.cpp"), this.buildSource(), 'utf8');
		await writeFile(join(outDir, "emoji.hpp"), this.buildHeader(), 'utf8');
		console.log(`Successfully generated ${headerDst}`);
		console.log(`Successfully generated ${sourceDst}`);
	}

	async tryCompile(outDir: string) {
		let old = process.cwd();
		process.chdir(outDir);
		console.log('Attempting compilation...');
		let result = spawnSync('/usr/bin/c++', ['emoji.cpp', '-c', '-o', 'emoji.o']);
		if (result.status != 0) {
			throw new Error(`Failed to compile: ${result.stderr.toString()}`);
		}
		process.chdir(old);
		console.log('Succesfully compiled, ready to embed');
	}
};
