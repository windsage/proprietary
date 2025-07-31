#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
perf_config.xml合法性验证脚本
验证Android性能配置文件的结构和内容是否符合规范
"""

import xml.etree.ElementTree as ET
import os
import sys
import re
from typing import List, Dict, Tuple, Optional


class PerfConfigValidator:
    """性能配置XML验证器"""

    def __init__(self):
        self.errors = []
        self.warnings = []
        self.valid_fps_values = {'30', '60', '90', '120', '144', 'common'}

    def validate_file(self, file_path: str) -> bool:
        """
        验证XML文件

        Args:
            file_path: XML文件路径

        Returns:
            bool: 验证是否通过
        """
        self.errors.clear()
        self.warnings.clear()

        # Check file existence
        if not os.path.exists(file_path):
            self.errors.append(f"文件不存在: {file_path}")
            return False

        # Parse XML
        try:
            tree = ET.parse(file_path)
            root = tree.getroot()
        except ET.ParseError as e:
            self.errors.append(f"XML解析错误: {str(e)}")
            return False
        except Exception as e:
            self.errors.append(f"文件读取错误: {str(e)}")
            return False

        # Start validation
        self._validate_root_node(root)
        self._validate_structure(root)

        return len(self.errors) == 0

    def _validate_root_node(self, root: ET.Element) -> None:
        """验证根节点"""
        if root.tag != 'configs':
            self.errors.append(f"根节点必须是'configs'，实际为'{root.tag}'")

    def _validate_structure(self, root: ET.Element) -> None:
        """验证整体结构"""
        packages = root.findall('package')

        if not packages:
            self.errors.append("根节点下必须至少有一个package节点")
            return

        package_names = set()

        for i, package in enumerate(packages):
            package_location = f"第{i+1}个package节点"

            # Validate package node
            package_name = self._validate_package_node(package, package_location)

            # Check for duplicate package names
            if package_name:
                if package_name in package_names:
                    self.errors.append(f"{package_location}: package名称'{package_name}'重复")
                else:
                    package_names.add(package_name)

            # Validate activities under this package
            self._validate_activities(package, package_location)

    def _validate_package_node(self, package: ET.Element, location: str) -> Optional[str]:
        """验证package节点"""
        if package.tag != 'package':
            self.errors.append(f"{location}: 节点类型应为'package'，实际为'{package.tag}'")
            return None

        # Check name attribute
        name = package.get('name')
        if not name:
            self.errors.append(f"{location}: package节点的name属性不能为空")
            return None

        name = name.strip()
        if not name:
            self.errors.append(f"{location}: package节点的name属性不能为空白字符")
            return None

        if name == 'common':
            self.errors.append(f"{location}: package节点的name不能为'common'")
            return None

        # Check for invalid characters in name
        if self._contains_invalid_xml_chars(name):
            self.errors.append(f"{location}: package名称'{name}'包含无效字符")

        return name

    def _validate_activities(self, package: ET.Element, package_location: str) -> None:
        """验证activity节点"""
        activities = package.findall('activity')

        if not activities:
            self.errors.append(f"{package_location}: package节点下必须至少有一个activity节点")
            return

        activity_names = set()

        for i, activity in enumerate(activities):
            activity_location = f"{package_location}的第{i+1}个activity节点"

            # Validate activity node
            activity_name = self._validate_activity_node(activity, activity_location)

            # Check for duplicate activity names within same package
            if activity_name:
                if activity_name in activity_names:
                    self.errors.append(f"{activity_location}: activity名称'{activity_name}'在同一package下重复")
                else:
                    activity_names.add(activity_name)

            # Validate fps nodes under this activity
            self._validate_fps_nodes(activity, activity_location)

    def _validate_activity_node(self, activity: ET.Element, location: str) -> Optional[str]:
        """验证activity节点"""
        if activity.tag != 'activity':
            self.errors.append(f"{location}: 节点类型应为'activity'，实际为'{activity.tag}'")
            return None

        # Check name attribute
        name = activity.get('name')
        if not name:
            self.errors.append(f"{location}: activity节点的name属性不能为空")
            return None

        name = name.strip()
        if not name:
            self.errors.append(f"{location}: activity节点的name属性不能为空白字符")
            return None

        # Check for invalid characters in name
        if self._contains_invalid_xml_chars(name):
            self.errors.append(f"{location}: activity名称'{name}'包含无效字符")

        return name

    def _validate_fps_nodes(self, activity: ET.Element, activity_location: str) -> None:
        """验证fps节点"""
        fps_nodes = activity.findall('fps')

        if not fps_nodes:
            self.errors.append(f"{activity_location}: activity节点下必须至少有一个fps节点")
            return

        fps_values = set()

        for i, fps in enumerate(fps_nodes):
            fps_location = f"{activity_location}的第{i+1}个fps节点"

            # Validate fps node
            fps_value = self._validate_fps_node(fps, fps_location)

            # Check for duplicate fps values within same activity
            if fps_value:
                if fps_value in fps_values:
                    self.warnings.append(f"{fps_location}: fps值'{fps_value}'在同一activity下重复")
                else:
                    fps_values.add(fps_value)

            # Validate perflock node under this fps
            self._validate_perflock_node(fps, fps_location)

    def _validate_fps_node(self, fps: ET.Element, location: str) -> Optional[str]:
        """验证fps节点"""
        if fps.tag != 'fps':
            self.errors.append(f"{location}: 节点类型应为'fps'，实际为'{fps.tag}'")
            return None

        # Check value attribute
        value = fps.get('value')
        if not value:
            self.errors.append(f"{location}: fps节点的value属性不能为空")
            return None

        value = value.strip()
        if not value:
            self.errors.append(f"{location}: fps节点的value属性不能为空白字符")
            return None

        if value not in self.valid_fps_values:
            valid_values = ', '.join(sorted(self.valid_fps_values))
            self.errors.append(f"{location}: fps值'{value}'无效，必须是以下值之一: {valid_values}")
            return None

        return value

    def _validate_perflock_node(self, fps: ET.Element, fps_location: str) -> None:
        """验证perflock节点"""
        perflock_nodes = fps.findall('perflock')

        if len(perflock_nodes) == 0:
            self.errors.append(f"{fps_location}: fps节点下必须有且只能有一个perflock节点")
            return
        elif len(perflock_nodes) > 1:
            self.errors.append(f"{fps_location}: fps节点下只能有一个perflock节点，实际有{len(perflock_nodes)}个")
            return

        perflock = perflock_nodes[0]
        perflock_location = f"{fps_location}的perflock节点"

        if perflock.tag != 'perflock':
            self.errors.append(f"{perflock_location}: 节点类型应为'perflock'，实际为'{perflock.tag}'")
            return

        # Validate perflock content
        perflock_text = perflock.text
        if not perflock_text:
            self.errors.append(f"{perflock_location}: perflock节点内容不能为空")
            return

        perflock_text = perflock_text.strip()
        if not perflock_text:
            self.errors.append(f"{perflock_location}: perflock节点内容不能为空白字符")
            return

        # Check if values are even count (comma separated)
        values = [v.strip() for v in perflock_text.split(',')]
        values = [v for v in values if v]  # Remove empty strings

        if len(values) % 2 != 0:
            self.errors.append(f"{perflock_location}: perflock节点中的值必须是偶数个，实际有{len(values)}个")

        # Check if all values are numeric (decimal or hexadecimal)
        for j, value in enumerate(values):
            if not self._is_valid_number(value):
                self.errors.append(f"{perflock_location}: 第{j+1}个值'{value}'不是有效的数字")

    def _is_valid_number(self, value: str) -> bool:
        """检查是否为有效数字（十进制或十六进制）"""
        if not value:
            return False

        # Check for hexadecimal number (0x prefix)
        if value.lower().startswith('0x'):
            hex_part = value[2:]
            if not hex_part:
                return False
            try:
                int(hex_part, 16)
                return True
            except ValueError:
                return False

        # Check for decimal number
        return value.isdigit()

    def _contains_invalid_xml_chars(self, text: str) -> bool:
        """检查是否包含XML无效字符"""
        invalid_chars = ['<', '>', '&', '"', "'"]
        return any(char in text for char in invalid_chars)

    def get_validation_report(self) -> str:
        """获取验证报告"""
        report = []

        if not self.errors and not self.warnings:
            report.append("✅ 验证通过！XML文件格式正确。")
        else:
            if self.errors:
                report.append(f"❌ 发现 {len(self.errors)} 个错误:")
                for i, error in enumerate(self.errors, 1):
                    report.append(f"  {i}. {error}")

            if self.warnings:
                report.append(f"\n⚠️  发现 {len(self.warnings)} 个警告:")
                for i, warning in enumerate(self.warnings, 1):
                    report.append(f"  {i}. {warning}")

        return '\n'.join(report)

    def get_error_count(self) -> int:
        """获取错误数量"""
        return len(self.errors)

    def get_warning_count(self) -> int:
        """获取警告数量"""
        return len(self.warnings)


def main():
    """主函数"""
    if len(sys.argv) != 2:
        print("使用方法: python perf_config_validator.py <xml_file_path>")
        sys.exit(1)

    xml_file = sys.argv[1]

    validator = PerfConfigValidator()
    is_valid = validator.validate_file(xml_file)

    print(validator.get_validation_report())

    if is_valid:
        print(f"\n文件 '{xml_file}' 验证通过！")
        sys.exit(0)
    else:
        print(f"\n文件 '{xml_file}' 验证失败！")
        print(f"错误数量: {validator.get_error_count()}")
        print(f"警告数量: {validator.get_warning_count()}")
        sys.exit(1)


if __name__ == "__main__":
    main()
